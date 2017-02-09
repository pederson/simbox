/** @file SimulationDataHDF.hpp
 *  @brief file with SimulationDataHDF class
 *
 *  This contains the SimulationDataHDF class
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _SIMULATIONDATAHDF_H
#define _SIMULATIONDATAHDF_H

#include "SimulationData.hpp"
#include "mpitools.hpp"
#include <hdf5.h>

namespace simbox{

/** @class SimulationDataHDF
 *  @brief class for I/O using HDF5 file format
 *
 *  Use this class to access and write data to/from
 *	the HDF5 file format. Parallel read/write is 
 *	possible with the HDF5 format using MPI
 *
 */
template<std::size_t dim>
class SimulationDataHDF : public SimulationData<dim>{
public:
	SimulationDataHDF(std::string filename, const Domain<dim> & dm, std::shared_ptr<const Mesh<dim>> mesh, std::vector<double> time)
	: m_dm(dm)
	, m_filename(filename)
	, m_mesh(mesh)
	, m_time(time)
	{

		// collectively open file
		m_plist_id = H5Pcreate(H5P_FILE_ACCESS);
		H5Pset_fapl_mpio(m_plist_id, MPI_COMM_WORLD, MPI_INFO_NULL);
		m_h5file = H5Fcreate(m_filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, m_plist_id);
		H5Pclose(m_plist_id);

		// create folder structure
		m_groupnames.push_back("Mesh");
		m_group_id["Mesh"] = H5Gcreate(m_h5file, "Mesh", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Time");
		m_group_id["Time"] = H5Gcreate(m_h5file, "Time", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Fields");
		m_group_id["Fields"] = H5Gcreate(m_h5file, "Fields", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Transients");
		m_group_id["Transients"] = H5Gcreate(m_h5file, "Transients", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

		write_HDF5_mesh();
		write_HDF5_time();
	}

	~SimulationDataHDF(){
		if (m_nodebuffer != nullptr) delete[] m_nodebuffer;
		if (m_cellbuffer != nullptr) delete[] m_cellbuffer;
		if (m_transbuffer != nullptr) delete[] m_transbuffer;

		H5Fclose(m_h5file);
	}

	// inspectors
	void print_summary(std::ostream & os = std::cout) const{
		os << "<SimulationData>" << "</SimulationData>" << std::endl;
	}

	std::shared_ptr<const Mesh<dim>> mesh() const {return m_mesh;};

	std::vector<double> time() const {return m_time;};

	std::vector<std::string> nodefields() const {return m_nodefields;};

	std::vector<std::string> elemfields() const {return m_elemfields;};

	std::vector<std::string> transients() const {return m_transients;};

	const double * get_nodefield(std::string fld, unsigned int tind) const {return nullptr;};
	const double * get_nodefield(std::string fld, double t) const {return nullptr;};
	const double * get_elemfield(std::string fld, unsigned int tind) const {return nullptr;};
	const double * get_elemfield(std::string fld, double t) const {return nullptr;};
	const double * get_transient(std::string tr) const {return nullptr;};

	bool field_exists(std::string f) const{
		for (auto i=0; i<m_nodefields.size(); i++){
			if (m_nodefields.at(i).compare(f) == 0){
				return true;
			}
		}
		for (auto i=0; i<m_elemfields.size(); i++){
			if (m_elemfields.at(i).compare(f) == 0){
				return true;
			}
		}
		return false;
	}

	// mutators
	void add_elemfield(std::string fld){
		m_elemfields.push_back(fld);
		m_group_id[fld] = m_group_id["Fields"];

		// create dataset for field
		m_dataspace_id[fld] = m_cellfield_space;
		m_dataset_id[fld] = H5Dcreate(m_group_id[fld], fld.c_str(), H5T_NATIVE_DOUBLE, m_dataspace_id[fld], H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	}

	void add_nodefield(std::string fld){
		m_nodefields.push_back(fld);
		m_group_id[fld] = m_group_id["Fields"];

		// create dataset for field
		m_dataspace_id[fld] = m_nodefield_space;
		m_dataset_id[fld] = H5Dcreate(m_group_id[fld], fld.c_str(), H5T_NATIVE_DOUBLE, m_dataspace_id[fld], H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	}

	void add_transient(std::string tr){
		m_transients.push_back(tr);
		m_group_id[tr] = m_group_id["Transients"];

		// create dataset for field
		m_dataspace_id[tr] = m_transient_space;
		m_dataset_id[tr] = H5Dcreate(m_group_id[tr], tr.c_str(), H5T_NATIVE_DOUBLE, m_dataspace_id[tr], H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	}
	void set_nodefield(std::string fld, unsigned int tind, const double * data){
		// copy to buffer
		if(!field_exists(fld)){
			cout << "ERROR: trying to write a field doesn't exist" << endl;
			throw -1;
		}

		// copy to buffer
		//memcpy(m_nodebuffer, data, m_mesh->nodecount()*sizeof(double));
		std::copy(data, data+m_mesh->nodecount(), m_nodebuffer);

		// get plist_id
		m_plist_id = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(m_plist_id, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset[2], count[2], stride[2], block[2];
		offset[0] = tind;	offset[1] = m_dm->array_node_offset(mpi::rank());
		count[0]  = 1;	count[1]  = m_mesh->nodecount();
		stride[0] = 1;	stride[1] = 1;
		block[0] = 1;	block[1] = 1;

		//cout << "Processor[" << mpi::rank() << "] writing data field " << fld << " with offset " << offset[1] << endl;

		H5Sselect_hyperslab(m_dataspace_id[fld], H5S_SELECT_SET, offset, stride, count, block);
		H5Dwrite(m_dataset_id[fld], H5T_NATIVE_DOUBLE, m_node_memspace, m_dataspace_id[fld], m_plist_id, m_nodebuffer);

	}

	void set_nodefield(std::string fld, double t, const double * data){
		set_nodefield(fld, get_time_index(t), data);
	}

	void set_elemfield(std::string fld, unsigned int tind, const double * data){
		// copy to buffer
		if(!field_exists(fld)){
			cout << "ERROR: trying to write a field doesn't exist" << endl;
			throw -1;
		}

		// copy to buffer
		//memcpy(m_cellbuffer, data, m_mesh->elementcount()*sizeof(double));
		std::copy(data, data+m_mesh->elementcount(), m_cellbuffer);

		// get plist_id
		m_plist_id = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(m_plist_id, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset[2], count[2], stride[2], block[2];
		offset[0] = tind;	offset[1] = m_dm->array_element_offset(mpi::rank());
		count[0]  = 1;	count[1]  = m_mesh->elementcount();
		stride[0] = 1;	stride[1] = 1;
		block[0] = 1;	block[1] = 1;

		H5Sselect_hyperslab(m_dataspace_id[fld], H5S_SELECT_SET, offset, stride, count, block);
		H5Dwrite(m_dataset_id[fld], H5T_NATIVE_DOUBLE, m_cell_memspace, m_dataspace_id[fld], m_plist_id, m_cellbuffer);

	}

	void set_elemfield(std::string fld, double t, const double * data){
		set_cellfield(fld, get_time_index(t), data);
	}

	void set_transient(std::string fld, const double * data){
		// copy to buffer
		//memcpy(m_transbuffer, data, m_time.size()*sizeof(double));
		std::copy(data, data+m_time.size(), m_transbuffer);

		// get plist_id
		m_plist_id = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(m_plist_id, H5FD_MPIO_COLLECTIVE);

		// write to file
		H5Dwrite(m_dataset_id[fld], H5T_NATIVE_DOUBLE, m_transient_space, m_transient_space, m_plist_id, m_transbuffer);

	}

	// optional tasks
	void write_XDMF(){
		if (!mpi::is_master()) return;

		std::ofstream ofs;
		ofs.open((m_filename.substr(0, m_filename.length()-3)+".xdmf"), ofstream::out | ofstream::trunc);

		write_XDMF_header(ofs);

		ofs << "<Xdmf Version=\"2.0\" xmlns:xi=\"http://www.w3.org/2001/XInclude\">" << endl;

		write_XDMF_DataItems(ofs);

		// write domain header
		ofs << "<Domain>" << endl;
		ofs << "\t<Grid Name=\"TimeSeries\" GridType=\"Collection\" CollectionType=\"Temporal\">" << endl;
		ofs << endl;

		// write time steps
		write_XDMF_timesteps(ofs);

		// close grid, domain, xdmf
		ofs << "</Grid>" << endl;
		ofs << "</Domain>" << endl;
		ofs << "</Xdmf>" << endl;

		ofs.close();
	}
	
private:

	unsigned int get_time_index(double t);

	// domain and mesh
	std::shared_ptr<Domain<dim>>		m_dm;
	std::shared_ptr<const Mesh<dim>> 	m_mesh;

	// field std::strings
	std::vector<std::string> 		m_nodefields;
	std::vector<std::string> 		m_elemfields;
	std::vector<std::string> 		m_transients;

	// time info
	std::vector<double> m_time;

	// HDF5 file resources
	std::string 	m_filename;
	hid_t 			m_h5file;
	hid_t			m_plist_id;
	hid_t			m_transient_space;
	hid_t			m_nodefield_space;
	hid_t			m_elemfield_space;
	hid_t			m_node_memspace;
	hid_t			m_elem_memspace;

	// HDF5 file access


	std::vector<std::string> 		m_groupnames;
	std::map<std::string, hid_t> 	m_group_id;	// group for each field
	std::map<std::string, hid_t> 	m_dataspace_id;
	std::map<std::string, hid_t>	m_slicespace_id;
	std::map<std::string, hid_t> 	m_dataset_id;

	double * m_nodebuffer;			// buffer for node data
	double * m_elembuffer;			// buffer for element data
	double * m_transbuffer;			// buffer for transient data


	void write_HDF5_mesh(){
			
		// set up dataspaces
		hsize_t dimcell[] = {m_time.size(), m_dm->nelements()};
		hsize_t dimnode[] = {m_time.size(), m_dm->nnodes()};
		hsize_t nnode_proc = m_mesh->nodecount();
		hsize_t ncell_proc = m_mesh->elementcount();
		m_cellfield_space = H5Screate_simple(2, dimcell, NULL);
		m_nodefield_space = H5Screate_simple(2, dimnode, NULL);
		m_cell_memspace = H5Screate_simple(1, &ncell_proc, NULL);
		m_node_memspace = H5Screate_simple(1, &nnode_proc, NULL);
		
		// create field buffers
		m_cellbuffer = new double[m_mesh->elementcount()];
		m_nodebuffer = new double[m_mesh->nodecount()];

		// set up subgroups
		m_groupnames.push_back("Nodes");
		m_group_id["Nodes"] = H5Gcreate(m_group_id["Mesh"], "Nodes", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Elements");
		m_group_id["Elements"] = H5Gcreate(m_group_id["Mesh"], "Elements", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

		// write node datasets
		hid_t node_set;
		hsize_t offset[1], count[1], stride[1], block[1];
		hsize_t nnodes = m_dm->nnodes();
		hid_t nodespace = H5Screate_simple(1, &nnodes, NULL);
		hid_t plist_id = H5Pcreate(H5P_DATASET_XFER);
	    H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_INDEPENDENT);
	    offset[0] = m_dm->array_node_offset(mpi::rank());
		count[0]  = m_mesh->nodecount();
		stride[0] = 1;
		block[0] = 1;

		// cout << "Total nodes: " << m_dm->nnodes() << " total cells: " << m_dm->nelements() << endl;
		// cout << "nnodes per element: " << m_mesh->element(0).num_vertices() << endl;
		// cout << "Processor " << mpi::rank() << " nnodes: " << nnode_proc << " ncell: " << ncell_proc << endl;
		// cout << "Processor " << mpi::rank() << " node offset: " << m_dm->array_node_offset(mpi::rank()) << " cell offset: " << m_dm->array_element_offset(mpi::rank()) << endl;
		// cout << "Processor " << mpi::rank() << " here" << endl;

		node_set = H5Dcreate(m_group_id["Nodes"], "NodesX", H5T_NATIVE_DOUBLE, nodespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		for (auto i=0; i<m_mesh->nodecount(); i++) m_nodebuffer[i] = m_mesh->node(i).x();
		H5Sselect_hyperslab(nodespace, H5S_SELECT_SET, offset, stride, count, block);
		H5Dwrite(node_set, H5T_NATIVE_DOUBLE, m_node_memspace, nodespace, plist_id, m_nodebuffer);

		if (m_mesh->num_dims() > 1){
			node_set = H5Dcreate(m_group_id["Nodes"], "NodesY", H5T_NATIVE_DOUBLE, nodespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->nodecount(); i++) m_nodebuffer[i] = m_mesh->node(i).y();
			H5Sselect_hyperslab(nodespace, H5S_SELECT_SET, offset, stride, count, block);
			H5Dwrite(node_set, H5T_NATIVE_DOUBLE, m_node_memspace, nodespace, plist_id, m_nodebuffer);
		}
		if (m_mesh->num_dims() > 2){
			node_set = H5Dcreate(m_group_id["Nodes"], "NodesZ", H5T_NATIVE_DOUBLE, nodespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->nodecount(); i++) m_nodebuffer[i] = m_mesh->node(i).z();
			H5Sselect_hyperslab(nodespace, H5S_SELECT_SET, offset, stride, count, block);
			H5Dwrite(node_set, H5T_NATIVE_DOUBLE, m_node_memspace, nodespace, plist_id, m_nodebuffer);
		}

		// cout << "Processor " << mpi::rank() << " wrote all nodes" << endl;


		
		// write element dataset
		unsigned int * ebuffer = new unsigned int[m_mesh->elementcount()];
		unsigned int nodeoff = m_dm->array_node_offset(mpi::rank());
		hsize_t nelem[2]; nelem[0] = m_dm->nelements(); nelem[1]=m_mesh->element(0).num_vertices();
		hid_t elemspace = H5Screate_simple(2, nelem, NULL);
		hsize_t offsete[2], counte[2], stridee[2], blocke[2];
		hid_t elem_set = H5Dcreate(m_group_id["Elements"], get_string(m_mesh->element(0).type()).c_str(), H5T_NATIVE_UINT, elemspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		// cout << "I have " << nelem[1] << " vertices per element" << endl;
		// cout << "and there are " << nelem[0] <<  " elements" << endl;
		for (auto j=0; j<nelem[1]; j++){
			offsete[0] = m_dm->array_element_offset(mpi::rank()); 	offsete[1] = j;
			counte[0]  = ncell_proc; 	counte[1]  = 1;
			stridee[0] = 1; 			stridee[1] = 1;
			blocke[0] = 1; 				blocke[1] = 1;
			H5Sselect_hyperslab(elemspace, H5S_SELECT_SET, offsete, stridee, counte, blocke);

			for (auto i=0; i<ncell_proc; i++){
				ebuffer[i] = m_mesh->element(i).vertex_ind(j) + nodeoff;
			} 
			H5Dwrite(elem_set, H5T_NATIVE_UINT, m_cell_memspace, elemspace, plist_id, ebuffer);
		}
		delete[] ebuffer;
		// cout << "Processor " << mpi::rank() << " wrote all elements" << endl;

		// insert nodedata
		vector<string> props = m_mesh->get_nodedata_names();
		m_groupnames.push_back("NodeData");
		m_group_id["NodeData"] = H5Gcreate(m_group_id["Mesh"], "NodeData", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		for (auto p=props.begin(); p!=props.end(); p++){

			//m_groupnames.push_back(*p);
			//m_group_id[*p] = H5Gcreate(m_group_id["NodeData"], (*p).c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			const double * dat = m_mesh->nodedata(*p);
			node_set = H5Dcreate(m_group_id["NodeData"], (*p).c_str(), H5T_NATIVE_DOUBLE, nodespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->nodecount(); i++) m_nodebuffer[i] = dat[i];
			H5Sselect_hyperslab(nodespace, H5S_SELECT_SET, offset, stride, count, block);
			H5Dwrite(node_set, H5T_NATIVE_DOUBLE, m_node_memspace, nodespace, plist_id, m_nodebuffer);
		}

		// insert elementdata
		hsize_t numelem = m_dm->nelements();
		// hsize_t nelemdat[2]; nelemdat[0] = m_dm->nelements(); nelemdat[1]=1;
		hid_t elemdatspace = H5Screate_simple(1, &numelem, NULL);
		offsete[0] = m_dm->array_element_offset(mpi::rank());
		// cout << "nelements total: " << numelem ;
		// cout << " offset: " << offsete[0] ;
		// cout << " nelem_local: " << m_mesh->elementcount() << endl;
		offsete[1] = 0;
		vector<string> eprops = m_mesh->get_elementdata_names();
		m_groupnames.push_back("ElementData");
		m_group_id["ElementData"] = H5Gcreate(m_group_id["Mesh"], "ElementData", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		for (auto p=eprops.begin(); p!=eprops.end(); p++){
			const double * dat = m_mesh->elementdata(*p);
			elem_set = H5Dcreate(m_group_id["ElementData"], (*p).c_str(), H5T_NATIVE_DOUBLE, elemdatspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->elementcount(); i++) m_cellbuffer[i] = dat[i];
			H5Sselect_hyperslab(elemdatspace, H5S_SELECT_SET, offsete, stridee, counte, blocke);
			H5Dwrite(elem_set, H5T_NATIVE_DOUBLE, m_cell_memspace, elemdatspace, plist_id, m_cellbuffer);
		}
	}

	void write_HDF5_time(){
			// create time buffer
		m_transbuffer = new double[m_time.size()];
		//memcpy(m_transbuffer, m_time.data(), m_time.size()*sizeof(double));
		std::copy(m_time.data(), m_time.data()+m_time.size(), m_transbuffer);
		
		// create dataspace for transient and time data
		hsize_t ntime = m_time.size();
		m_transient_space = H5Screate_simple(1, &ntime, NULL);

		// create dataset for time data
		m_dataspace_id["Time"] = m_transient_space;
		m_dataset_id["Time"] = H5Dcreate(m_group_id["Time"], "Time", H5T_NATIVE_DOUBLE, m_dataspace_id["Time"], H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

		// create property list for collective dataset write
		m_plist_id = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(m_plist_id, H5FD_MPIO_COLLECTIVE);

		H5Dwrite(m_dataset_id["Time"], H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, m_plist_id, m_transbuffer);
		H5Pclose(m_plist_id);
	}

	// XDMF file access
	void write_XDMF_header(std::ofstream & ofs){
		ofs << "<?xml version=\"1.0\" ?>" << endl;
		ofs << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" [" << endl;
		ofs << "\t<!ENTITY HDF5FILE \"" << m_filename << "\">" << endl;
		ofs << "\t<!ENTITY nvertices \"" << m_dm->nnodes() << "\">" << endl;
		ofs << "\t<!ENTITY nelements \"" << m_dm->nelements() << "\">" << endl;
		ofs << "\t<!ENTITY ntsteps \"" << m_time.size() << "\">" << endl;
		ofs << "]>" << endl;
	}

	void write_XDMF_DataItems(std::ofstream & ofs){
		// mesh data items
		ofs << "\n\n<!--===================== Mesh Data Items ====================-->" << endl;
		ofs << "<DataItem Name=\"NodesX\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << endl;
		ofs << "\t&HDF5FILE;:/Mesh/Nodes/NodesX" << endl;
		ofs << "</DataItem>" << endl;
		if (m_mesh->num_dims()>1){
			ofs << "<DataItem Name=\"NodesY\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << endl;
			ofs << "\t&HDF5FILE;:/Mesh/Nodes/NodesY" << endl;
			ofs << "</DataItem>" << endl;
		}
		if (m_mesh->num_dims()>2){
			ofs << "<DataItem Name=\"NodesZ\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << endl;
			ofs << "\t&HDF5FILE;:/Mesh/Nodes/NodesZ" << endl;
			ofs << "</DataItem>" << endl;
		}

		// element data items
		string eltype = get_string(m_mesh->element(0).type());
		ofs << "<DataItem Name=\"" << eltype << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements; " << get_nvert(m_mesh->element(0).type()) << "\">" << endl;
		ofs << "\t&HDF5FILE;:/Mesh/Elements/" << eltype << endl;
		ofs << "</DataItem>" << endl;

		// nodedata
		vector<string> props = m_mesh->get_nodedata_names();
		for (auto p=props.begin(); p!=props.end(); p++){
			ofs << "<DataItem Name=\"" << *p << "_Node" << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << endl;
			ofs << "\t&HDF5FILE;:/Mesh/NodeData/" << *p << endl;
			ofs << "</DataItem>" << endl;
		}

		// elementdata
		vector<string> eprops = m_mesh->get_elementdata_names();
		for (auto p=eprops.begin(); p!=eprops.end(); p++){
			ofs << "<DataItem Name=\"" << *p << "_Cell" << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements;\">" << endl;
			ofs << "\t&HDF5FILE;:/Mesh/ElementData/" << *p << endl;
			ofs << "</DataItem>" << endl;
		}


		// Field Data items
		ofs << "\n\n<!--===================== Field Data Items ===================-->" << endl;
		for (auto i=0; i<m_nodefields.size(); i++){
			ofs << "<DataItem Name=\"" << m_nodefields[i] << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&ntsteps; &nvertices;\">" << endl;
			ofs << "\t&HDF5FILE;:/Fields/" << m_nodefields[i] << endl;
			ofs << "</DataItem>" << endl;
		}
		for (auto i=0; i<m_elemfields.size(); i++){
			ofs << "<DataItem Name=\"" << m_elemfields[i] << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&ntsteps; &nelements;\">" << endl;
			ofs << "\t&HDF5FILE;:/Fields/" << m_elemfields[i] << endl;
			ofs << "</DataItem>" << endl;
		}

	}
	void write_XDMF_timesteps(std::ofstream & ofs){
		for (auto i=0; i<m_time.size(); i++){
			ofs << "<!--===========================TIME STEP=======================-->" << endl;
			ofs << "<Grid Name=\"FDTDGrid\" GridType=\"Uniform\">" << endl;
			ofs << "<Time Value=\"" << m_time[i] << "\"/>" << endl;

			// topology
			ofs << "<Topology TopologyType=\"" << xdmf_topology[(int)m_mesh->element(0).type()] << "\" NumberOfElements=\"&nelements;\">" << endl;
			write_XDMF_reference(ofs, get_string(m_mesh->element(0).type()));
			ofs << "</Topology>" << endl;

			// geometry
			if (m_mesh->num_dims()==1){
				ofs << "<Geometry GeometryType=\"X\">" << endl;
			}
			else if (m_mesh->num_dims() == 2){
				ofs << "<Geometry GeometryType=\"X_Y\">" << endl;
			}
			else if (m_mesh->num_dims() == 3){
				ofs << "<Geometry GeometryType=\"X_Y_Z\">" << endl;
			}
			write_XDMF_reference(ofs, "NodesX");
			if (m_mesh->num_dims() > 1){
				write_XDMF_reference(ofs, "NodesY");
			}
			if (m_mesh->num_dims() > 2){
				write_XDMF_reference(ofs, "NodesZ");
			}
			ofs << "</Geometry>" << endl;

			// attributes
			vector<string> props = m_mesh->get_nodedata_names();
			for (auto p=props.begin(); p!=props.end(); p++){
				ofs << "<Attribute Name=\"" << *p << "\" AttributeType=\"Scalar\" Center=\"Node\">" << endl;
				ofs << "<DataItem Reference=\"XML\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << endl;
				ofs << "\t/Xdmf/DataItem[@Name=\"" << *p << "_Node" << "\"]" << endl;
				ofs << "</DataItem>" << endl;
				ofs << "</Attribute>" << endl;
			}
			vector<string> eprops = m_mesh->get_elementdata_names();
			for (auto p=eprops.begin(); p!=eprops.end(); p++){
				ofs << "<Attribute Name=\"" << *p << "\" AttributeType=\"Scalar\" Center=\"Cell\">" << endl;
				ofs << "<DataItem Reference=\"XML\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements;\">" << endl;
				ofs << "\t/Xdmf/DataItem[@Name=\"" << *p << "_Cell" << "\"]" << endl;
				ofs << "</DataItem>" << endl;
				ofs << "</Attribute>" << endl;
			}
			for (auto f=0; f<m_nodefields.size(); f++){
				ofs << "<Attribute Name=\"" << m_nodefields[f] << "\" AttributeType=\"Scalar\" Center=\"Node\">" << endl;
				ofs << "<DataItem ItemType=\"HyperSlab\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << endl;
				ofs << "<DataItem Dimensions=\"3 2\" Format=\"XML\">" << endl;
				ofs << i << " 0" << endl;
				ofs << "1 1" << endl;
				ofs << "1 &nvertices;" << endl;
				ofs << "</DataItem>" << endl;
				write_XDMF_reference(ofs, m_nodefields[f]);
				ofs << "</DataItem>" << endl;
				ofs << "</Attribute>" << endl;
			}
			for (auto f=0; f<m_elemfields.size(); f++){
				ofs << "<Attribute Name=\"" << m_elemfields[f] << "\" AttributeType=\"Scalar\" Center=\"Cell\">" << endl;
				ofs << "<DataItem ItemType=\"HyperSlab\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements;\">" << endl;
				ofs << "<DataItem Dimensions=\"3 2\" Format=\"XML\">" << endl;
				ofs << i << " 0" << endl;
				ofs << "1 1" << endl;
				ofs << "1 &nelements;" << endl;
				ofs << "</DataItem>" << endl;
				write_XDMF_reference(ofs, m_elemfields[f]);
				ofs << "</DataItem>" << endl;
				ofs << "</Attribute>" << endl;
			}
			ofs << "</Grid>" << endl;

		}
	}

	void write_XDMF_reference(std::ofstream & ofs, std::string refname){
		ofs << "<DataItem Reference=\"XML\">" << endl;
		ofs << "\t/Xdmf/DataItem[@Name=\"" << refname << "\"]" << endl;
		ofs << "</DataItem>" << endl;
	}

};

}
#endif
