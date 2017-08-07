/** @file SimulationDataHDF.hpp
 *  @brief file with SimulationDataHDF class
 *
 *  This contains the SimulationDataHDF class
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _HDFFILESYSTEMWRITER_H
#define _HDFFILESYSTEMWRITER_H

#include <string>
#include <vector>
// #include <fstream>
#include <iostream>


// HDF5 library //
#include <hdf5.h>
//////////////////

namespace simbox{



// list out datatypes
template <typename T>
struct H5DataType{
	// throw an error if this is instantiated instead of one of the specializations
	// static_assert(false, "H5DataType must have an overloaded template argument");
	static const hid_t value;
};
template<> const hid_t H5DataType<char>::value 				= H5T_NATIVE_CHAR;
template<> const hid_t H5DataType<signed char>::value 		= H5T_NATIVE_SCHAR;
template<> const hid_t H5DataType<unsigned char>::value 	= H5T_NATIVE_UCHAR;
template<> const hid_t H5DataType<short>::value 			= H5T_NATIVE_SHORT;
template<> const hid_t H5DataType<unsigned short>::value 	= H5T_NATIVE_USHORT;
template<> const hid_t H5DataType<int>::value 				= H5T_NATIVE_INT;
template<> const hid_t H5DataType<unsigned int>::value 		= H5T_NATIVE_UINT;
template<> const hid_t H5DataType<long>::value 				= H5T_NATIVE_LONG;
template<> const hid_t H5DataType<unsigned long>::value 	= H5T_NATIVE_ULONG;
template<> const hid_t H5DataType<long long>::value 		= H5T_NATIVE_LLONG;
template<> const hid_t H5DataType<unsigned long long>::value = H5T_NATIVE_ULLONG;
template<> const hid_t H5DataType<float>::value 			= H5T_NATIVE_FLOAT;
template<> const hid_t H5DataType<double>::value 			= H5T_NATIVE_DOUBLE;
template<> const hid_t H5DataType<long double>::value 		= H5T_NATIVE_LDOUBLE;
// template<> const hid_t H5DataType<hsize_t>::value 			= H5T_NATIVE_HSIZE;
// template<> const hid_t H5DataType<hssize_t>::value 			= H5T_NATIVE_HSSIZE;
// template<> const hid_t H5DataType<herr_t>::value 			= H5T_NATIVE_HERR;
// template<> const hid_t H5DataType<hbool_t>::value 			= H5T_NATIVE_HBOOL;


// template <> struct H5DataType<double>{static const hid_t value = H5T_NATIVE_DOUBLE;};
// template <> struct H5DataType<unsigned int>{typedef H5T_NATIVE_UINT type;};

/** @class HDFFileSystemWriter
 *  @brief class for I/O using HDF5 file format
 *
 *  Use this class to access and write data to/from
 *	the HDF5 file format. Parallel read/write is 
 *	possible with the HDF5 format using MPI
 *
 */
class HDFFileSystemWriter{
public:
	HDFFileSystemWriter(std::string filename)
	: mFileName(filename)
	{
		// collectively open file
		mPlistId = H5Pcreate(H5P_FILE_ACCESS);
		// if MPI enabled...
		// H5Pset_fapl_mpio(mPlistId, MPI_COMM_WORLD, MPI_INFO_NULL);
		mH5File = H5Fcreate(mFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, mPlistId);
		H5Pclose(mPlistId);
	};

	// here, the Args arguments specify the path to the file
	// in which we want to read/write
	template <typename T, typename... Args>
	bool write(T * data, hsize_t length, Args... args){
		hid_t ds = makeDataspace(1, &length);
		hid_t set = makeDataset(mH5File, args..., H5DataType<T>::value, ds);
		hid_t ms = makeDataspace(1, &length);

		// get propertylist ID
		mPlistId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(mPlistId, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset = 0, count = length, stride = 1, block = 1;
		H5Sselect_hyperslab(ds, H5S_SELECT_SET, &offset, &stride, &count, &block);
		H5Dwrite(set, H5DataType<T>::value, ms, ds, mPlistId, data);
		return true;
	}


	template <typename T, typename... Args>
	bool read(T * target, hsize_t length, Args... args){
		hid_t ds = makeDataspace(1, &length);
		hid_t set = openDataset(mH5File, args...);
		// hid_t set = makeDataset(mH5File, args..., H5DataType<T>::value, ds);
		hid_t ms = makeDataspace(1, &length);

		// get propertylist ID
		mPlistId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(mPlistId, H5FD_MPIO_INDEPENDENT);

		H5Dread(set, H5DataType<T>::value, ms, ds, mPlistId, target);
		return true;
	}

	void close(){
		H5Fclose(mH5File);
	};


private:
	std::string 		mFileName;

	hid_t 			mH5File;
	hid_t			mPlistId;
	hid_t			mTransientSpace;
	hid_t			mNodefieldSpace;
	hid_t			mElementfieldspace;
	hid_t			mNodeMemspace;
	hid_t			mElemMemspace;






	// make a new dataset at a given location, and return an identifier to the dataset
	template <typename... Path>
	hid_t makeDataset(hid_t folder, std::string first, Path... pth){
		hid_t newfolder;
		if (checkExists(folder, first) > 0) newfolder = H5Gopen(folder, first.c_str(), H5P_DEFAULT);
		else newfolder = makeFolder(folder, first);
		return makeDataset(newfolder, pth...);
	}

	// make a new folder at a given location and return an indentifier to the folder
	template <typename... Path>
	hid_t makeFolder(hid_t basefolder, std::string first, Path... pth){
		hid_t newbase;
		if (checkExists(basefolder, first) > 0) newbase = H5Gopen(basefolder, first.c_str(), H5P_DEFAULT);
		else newbase = makeFolder(basefolder, first);
		return makeFolder(newbase, pth...);
	}


	// open an existing dataset
	template <typename... Path>
	hid_t openDataset(hid_t folder, std::string first, Path... pth){
		hid_t newfolder;
		if (checkExists(folder, first) > 0) newfolder = H5Gopen(folder, first.c_str(), H5P_DEFAULT);
		else{
			std::cout << "HDFFileSystemWriter::ERROR folder does not exist in HDF file" << std::endl;
			throw -1;
		}
		return openDataset(newfolder, pth...);
	}

	



	///////////// Wrappers for HDF5 functions ///////////////

	// make a new dataset at a given location, and return an identifier to the dataset
	hid_t makeDataset(hid_t folder, std::string dataname, hid_t datatype, hid_t dataspace, 
					  hid_t link_creation_plist_id = H5P_DEFAULT,
					  hid_t group_creation_plist_id = H5P_DEFAULT,
					  hid_t group_access_plist_id = H5P_DEFAULT){
		return H5Dcreate(folder, dataname.c_str(), datatype, dataspace, link_creation_plist_id, group_creation_plist_id, group_access_plist_id);
	}

	// make a folder at a given location, and return an identifier to the new folder
	// this will fail if the folder already exists
	hid_t makeFolder(hid_t folderlocation, std::string foldername, 
					 hid_t link_creation_plist_id = H5P_DEFAULT,
					 hid_t group_creation_plist_id = H5P_DEFAULT,
					 hid_t group_access_plist_id = H5P_DEFAULT)
	{
		return H5Gcreate(folderlocation, foldername.c_str(), link_creation_plist_id, group_creation_plist_id, group_access_plist_id);
	}


	hid_t makeDataspace(int rank, const hsize_t * dims, const hsize_t * maxdims = NULL){
		return H5Screate_simple(rank, dims, maxdims);
	}

	// check if a file or folder exists
	// if > 0, it exists
	// if == 0, it doesn't exist
	// if < 0, there was some other error
	hid_t checkExists(hid_t location, std::string name, hid_t link_access_plist_id = H5P_DEFAULT){
		return H5Lexists(location, name.c_str(), link_access_plist_id);
	}


	// open an existing dataset and return its identifier
	hid_t openDataset(hid_t folder, std::string dataname, hid_t dataset_access_plist_id = H5P_DEFAULT){
		return H5Dopen(folder, dataname.c_str(), dataset_access_plist_id);
	}
};



/*
template<std::size_t dim>
class SimulationDataHDF : public SimulationData<dim>{
public:
	SimulationDataHDF(std::string filename, const Domain<dim> & dm, std::shared_ptr<const Mesh<dim>> mesh, std::vector<double> time)
	: m_dm(&dm)
	, m_filename(filename)
	, m_mesh(mesh)
	, m_time(time)
	{

		// collectively open file
		m_plist_id = H5Pcreate(H5P_FILE_ACCESS);
		H5Pset_fapl_mpio(m_plist_id, MPI_COMM_WORLD, MPI_INFO_NULL);
		m_h5file = H5Fcreate(m_filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, m_plist_id);
		H5Pclose(m_plist_id);

		// get name for this proc (pad with zeros accordingly)
		m_proc_name = "p";
		m_proc_name += std::to_string(mpi::rank());

		// create folder structure
		m_groupnames.push_back(m_proc_name);
		m_group_id[m_proc_name] = H5Gcreate(m_h5file, m_proc_name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Mesh");
		m_group_id["Mesh"] = H5Gcreate(m_group_id[m_proc_name], "Mesh", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Fields");
		m_group_id["Fields"] = H5Gcreate(m_group_id[m_proc_name], "Fields", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);


		m_groupnames.push_back("Time");
		m_group_id["Time"] = H5Gcreate(m_h5file, "Time", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Transients");
		m_group_id["Transients"] = H5Gcreate(m_h5file, "Transients", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

		write_HDF5_mesh();
		write_HDF5_time();
	}

	~SimulationDataHDF(){
		if (m_nodebuffer != nullptr) delete[] m_nodebuffer;
		if (m_elembuffer != nullptr) delete[] m_elembuffer;
		if (m_transbuffer != nullptr) delete[] m_transbuffer;

		H5Fclose(m_h5file);
	}

	// inspectors
	void print_summary(std::ostream & os = std::cout) const{
		os << "<SimulationDataHDF>\"" << m_filename << "\"</SimulationDataHDF>" << std::endl;
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
		m_dataspace_id[fld] = m_elemfield_space;
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
			std::cout << "SimulationDataHDF: trying to write a field doesn't exist" << std::endl;
			throw -1;
		}

		// copy to buffer
		//memcpy(m_nodebuffer, data, m_mesh->nodecount()*sizeof(double));
		std::copy(data, data+m_mesh->snodecount(), m_nodebuffer);

		// get plist_id
		m_plist_id = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(m_plist_id, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset[2], count[2], stride[2], block[2];
		offset[0] = tind;	offset[1] = 0;//m_dm->array_node_offset(mpi::rank());
		count[0]  = 1;	count[1]  = m_mesh->snodecount();
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
			std::cout << "SimulationDataHDF: trying to write a field doesn't exist" << std::endl;
			throw -1;
		}

		// copy to buffer
		//memcpy(m_elembuffer, data, m_mesh->elementcount()*sizeof(double));
		std::copy(data, data+m_mesh->selementcount(), m_elembuffer);

		// get plist_id
		m_plist_id = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(m_plist_id, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset[2], count[2], stride[2], block[2];
		offset[0] = tind;	offset[1] = 0;//m_dm->array_element_offset(mpi::rank());
		count[0]  = 1;	count[1]  = m_mesh->selementcount();
		stride[0] = 1;	stride[1] = 1;
		block[0] = 1;	block[1] = 1;

		H5Sselect_hyperslab(m_dataspace_id[fld], H5S_SELECT_SET, offset, stride, count, block);
		H5Dwrite(m_dataset_id[fld], H5T_NATIVE_DOUBLE, m_elem_memspace, m_dataspace_id[fld], m_plist_id, m_elembuffer);

	}

	void set_elemfield(std::string fld, double t, const double * data){
		set_elemfield(fld, get_time_index(t), data);
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
		ofs.open((m_filename.substr(0, m_filename.length()-3)+".xdmf"), std::ofstream::out | std::ofstream::trunc);

		write_XDMF_header(ofs);

		ofs << "<Xdmf Version=\"2.0\" xmlns:xi=\"http://www.w3.org/2001/XInclude\">" << std::endl;

		write_XDMF_DataItems(ofs);

		// write domain header
		ofs << "<Domain>" << std::endl;
		ofs << "\t<Grid Name=\"TimeSeries\" GridType=\"Collection\" CollectionType=\"Temporal\">" << std::endl;
		ofs << std::endl;

		// write time steps
		write_XDMF_timesteps(ofs);

		// close grid, domain, xdmf
		ofs << "</Grid>" << std::endl;
		ofs << "</Domain>" << std::endl;
		ofs << "</Xdmf>" << std::endl;

		ofs.close();
	}
	
private:

	unsigned int get_time_index(double t) const{
		for (auto i=0; i<m_time.size(); i++){ 
			if (m_time[i] == t){
				return i;
			}
		}
		std::cerr << "SimulationDataHDF: that time slot doesn't exist" << std::endl;
		throw -1;
	}

	// domain and mesh
	const Domain<dim> * 				m_dm;
	// std::shared_ptr<Domain<dim>>		m_dm;
	std::shared_ptr<const Mesh<dim>> 	m_mesh;

	// field std::strings
	std::vector<std::string> 		m_nodefields;
	std::vector<std::string> 		m_elemfields;
	std::vector<std::string> 		m_transients;

	// time info
	std::vector<double> m_time;

	// HDF5 file resources
	std::string 	m_filename;
	std::string 	m_proc_name;
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
		hsize_t dimelem[] = {m_time.size(), m_dm->elementcount()};
		hsize_t dimnode[] = {m_time.size(), m_dm->nodecount()};
		hsize_t nnode_proc = m_mesh->snodecount();
		hsize_t nelem_proc = m_mesh->selementcount();
		m_elemfield_space = H5Screate_simple(2, dimelem, NULL);
		m_nodefield_space = H5Screate_simple(2, dimnode, NULL);
		m_elem_memspace = H5Screate_simple(1, &nelem_proc, NULL);
		m_node_memspace = H5Screate_simple(1, &nnode_proc, NULL);
		
		// create field buffers
		m_elembuffer = new double[m_mesh->selementcount()];
		m_nodebuffer = new double[m_mesh->snodecount()];

		// set up subgroups
		m_groupnames.push_back("Nodes");
		m_group_id["Nodes"] = H5Gcreate(m_group_id["Mesh"], "Nodes", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		m_groupnames.push_back("Elements");
		m_group_id["Elements"] = H5Gcreate(m_group_id["Mesh"], "Elements", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

		// write node datasets
		hid_t node_set;
		hsize_t offset[1], count[1], stride[1], block[1];
		hsize_t nnodes = m_dm->nodecount();
		hid_t nodespace = H5Screate_simple(1, &nnodes, NULL);
		hid_t plist_id = H5Pcreate(H5P_DATASET_XFER);
	    H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_INDEPENDENT);
	    offset[0] = 0;//m_dm->array_node_offset(mpi::rank());
		count[0]  = m_mesh->snodecount();
		stride[0] = 1;
		block[0] = 1;

		// cout << "Total nodes: " << m_dm->nnodes() << " total cells: " << m_dm->nelements() << std::endl;
		// cout << "nnodes per element: " << m_mesh->element(0).num_vertices() << std::endl;
		// cout << "Processor " << mpi::rank() << " nnodes: " << nnode_proc << " ncell: " << nelem_proc << std::endl;
		// cout << "Processor " << mpi::rank() << " node offset: " << m_dm->array_node_offset(mpi::rank()) << " cell offset: " << m_dm->array_element_offset(mpi::rank()) << std::endl;
		// cout << "Processor " << mpi::rank() << " here" << std::endl;

		for (auto j=0; j<dim; j++){
			node_set = H5Dcreate(m_group_id["Nodes"], ("X"+std::to_string(j)).c_str(), H5T_NATIVE_DOUBLE, nodespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->snodecount(); i++) m_nodebuffer[i] = m_mesh->snode(i).x[j];
			H5Sselect_hyperslab(nodespace, H5S_SELECT_SET, offset, stride, count, block);
			H5Dwrite(node_set, H5T_NATIVE_DOUBLE, m_node_memspace, nodespace, plist_id, m_nodebuffer);
		}

		// cout << "Processor " << mpi::rank() << " wrote all nodes" << std::endl;


		
		// write element dataset
		unsigned int * ebuffer = new unsigned int[m_mesh->selementcount()];
		unsigned int nodeoff = 0;//m_dm->array_node_offset(mpi::rank());
		hsize_t nelem[2]; nelem[0] = m_dm->elementcount(); nelem[1]=m_mesh->selement(0).nodeinds.size();
		hid_t elemspace = H5Screate_simple(2, nelem, NULL);
		hsize_t offsete[2], counte[2], stridee[2], blocke[2];
		hid_t elem_set = H5Dcreate(m_group_id["Elements"], get_string(m_mesh->selement(0).type).c_str(), H5T_NATIVE_UINT, elemspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		// cout << "I have " << nelem[1] << " vertices per element" << std::endl;
		// cout << "and there are " << nelem[0] <<  " elements" << std::endl;
		for (auto j=0; j<nelem[1]; j++){
			offsete[0] = 0; 			offsete[1] = j;
			counte[0]  = nelem_proc; 	counte[1]  = 1;
			stridee[0] = 1; 			stridee[1] = 1;
			blocke[0] = 1; 				blocke[1] = 1;
			H5Sselect_hyperslab(elemspace, H5S_SELECT_SET, offsete, stridee, counte, blocke);

			for (auto i=0; i<nelem_proc; i++){
				ebuffer[i] = m_mesh->selement(i).nodeinds[j] + nodeoff;
			} 
			H5Dwrite(elem_set, H5T_NATIVE_UINT, m_elem_memspace, elemspace, plist_id, ebuffer);
		}
		delete[] ebuffer;
		// cout << "Processor " << mpi::rank() << " wrote all elements" << std::endl;

		// insert nodedata
		std::vector<std::string> props = m_mesh->get_nodedata_names();
		m_groupnames.push_back("NodeData");
		m_group_id["NodeData"] = H5Gcreate(m_group_id["Mesh"], "NodeData", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		for (auto p=props.begin(); p!=props.end(); p++){

			//m_groupnames.push_back(*p);
			//m_group_id[*p] = H5Gcreate(m_group_id["NodeData"], (*p).c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			const double * dat = &(m_mesh->nodedata(*p).front());
			node_set = H5Dcreate(m_group_id["NodeData"], (*p).c_str(), H5T_NATIVE_DOUBLE, nodespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->snodecount(); i++) m_nodebuffer[i] = dat[i];
			H5Sselect_hyperslab(nodespace, H5S_SELECT_SET, offset, stride, count, block);
			H5Dwrite(node_set, H5T_NATIVE_DOUBLE, m_node_memspace, nodespace, plist_id, m_nodebuffer);
		}

		// insert elementdata
		hsize_t numelem = m_dm->elementcount();
		// hsize_t nelemdat[2]; nelemdat[0] = m_dm->nelements(); nelemdat[1]=1;
		hid_t elemdatspace = H5Screate_simple(1, &numelem, NULL);
		offsete[0] = 0;//m_dm->array_element_offset(mpi::rank());
		// cout << "nelements total: " << numelem ;
		// cout << " offset: " << offsete[0] ;
		// cout << " nelem_local: " << m_mesh->elementcount() << std::endl;
		offsete[1] = 0;
		std::vector<std::string> eprops = m_mesh->get_elementdata_names();
		m_groupnames.push_back("ElementData");
		m_group_id["ElementData"] = H5Gcreate(m_group_id["Mesh"], "ElementData", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		for (auto p=eprops.begin(); p!=eprops.end(); p++){
			const double * dat = &(m_mesh->elementdata(*p).front());
			elem_set = H5Dcreate(m_group_id["ElementData"], (*p).c_str(), H5T_NATIVE_DOUBLE, elemdatspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
			for (auto i=0; i<m_mesh->selementcount(); i++) m_elembuffer[i] = dat[i];
			H5Sselect_hyperslab(elemdatspace, H5S_SELECT_SET, offsete, stridee, counte, blocke);
			H5Dwrite(elem_set, H5T_NATIVE_DOUBLE, m_elem_memspace, elemdatspace, plist_id, m_elembuffer);
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
		ofs << "<?xml version=\"1.0\" ?>" << std::endl;
		ofs << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" [" << std::endl;
		ofs << "\t<!ENTITY HDF5FILE \"" << m_filename << "\">" << std::endl;
		ofs << "\t<!ENTITY nvertices \"" << m_dm->nodecount() << "\">" << std::endl;
		ofs << "\t<!ENTITY nelements \"" << m_dm->elementcount() << "\">" << std::endl;
		ofs << "\t<!ENTITY ntsteps \"" << m_time.size() << "\">" << std::endl;
		ofs << "]>" << std::endl;
	}

	void write_XDMF_DataItems(std::ofstream & ofs){
		// mesh data items
		ofs << "\n\n<!--===================== Mesh Data Items ====================-->" << std::endl;
		for (auto j=0; j<dim; j++){
			ofs << "<DataItem Name=\"X" << j << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << std::endl;
			ofs << "\t&HDF5FILE;:/Mesh/Nodes/X" << j << std::endl;
			ofs << "</DataItem>" << std::endl;
		}

		// element data items
		std::string eltype = get_string(m_mesh->selement(0).type);
		ofs << "<DataItem Name=\"" << eltype << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements; " << get_nvert(m_mesh->selement(0).type) << "\">" << std::endl;
		ofs << "\t&HDF5FILE;:/Mesh/Elements/" << eltype << std::endl;
		ofs << "</DataItem>" << std::endl;

		// nodedata
		std::vector<std::string> props = m_mesh->get_nodedata_names();
		for (auto p=props.begin(); p!=props.end(); p++){
			ofs << "<DataItem Name=\"" << *p << "_Node" << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << std::endl;
			ofs << "\t&HDF5FILE;:/Mesh/NodeData/" << *p << std::endl;
			ofs << "</DataItem>" << std::endl;
		}

		// elementdata
		std::vector<std::string> eprops = m_mesh->get_elementdata_names();
		for (auto p=eprops.begin(); p!=eprops.end(); p++){
			ofs << "<DataItem Name=\"" << *p << "_Cell" << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements;\">" << std::endl;
			ofs << "\t&HDF5FILE;:/Mesh/ElementData/" << *p << std::endl;
			ofs << "</DataItem>" << std::endl;
		}


		// Field Data items
		ofs << "\n\n<!--===================== Field Data Items ===================-->" << std::endl;
		for (auto i=0; i<m_nodefields.size(); i++){
			ofs << "<DataItem Name=\"" << m_nodefields[i] << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&ntsteps; &nvertices;\">" << std::endl;
			ofs << "\t&HDF5FILE;:/Fields/" << m_nodefields[i] << std::endl;
			ofs << "</DataItem>" << std::endl;
		}
		for (auto i=0; i<m_elemfields.size(); i++){
			ofs << "<DataItem Name=\"" << m_elemfields[i] << "\" Format=\"HDF\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&ntsteps; &nelements;\">" << std::endl;
			ofs << "\t&HDF5FILE;:/Fields/" << m_elemfields[i] << std::endl;
			ofs << "</DataItem>" << std::endl;
		}

	}
	void write_XDMF_timesteps(std::ofstream & ofs){
		for (auto i=0; i<m_time.size(); i++){
			ofs << "<!--===========================TIME STEP=======================-->" << std::endl;
			ofs << "<Grid Name=\"FDTDGrid\" GridType=\"Uniform\">" << std::endl;
			ofs << "<Time Value=\"" << m_time[i] << "\"/>" << std::endl;

			// topology
			ofs << "<Topology TopologyType=\"" << xdmf_topology[(int)m_mesh->selement(0).type] << "\" NumberOfElements=\"&nelements;\">" << std::endl;
			write_XDMF_reference(ofs, get_string(m_mesh->selement(0).type));
			ofs << "</Topology>" << std::endl;

			// geometry
			if (dim==1){
				ofs << "<Geometry GeometryType=\"X\">" << std::endl;
			}
			else if (dim == 2){
				ofs << "<Geometry GeometryType=\"X_Y\">" << std::endl;
			}
			else if (dim == 3){
				ofs << "<Geometry GeometryType=\"X_Y_Z\">" << std::endl;
			}
			write_XDMF_reference(ofs, "NodesX");
			if (dim > 1){
				write_XDMF_reference(ofs, "NodesY");
			}
			if (dim > 2){
				write_XDMF_reference(ofs, "NodesZ");
			}
			ofs << "</Geometry>" << std::endl;

			// attributes
			std::vector<std::string> props = m_mesh->get_nodedata_names();
			for (auto p=props.begin(); p!=props.end(); p++){
				ofs << "<Attribute Name=\"" << *p << "\" AttributeType=\"Scalar\" Center=\"Node\">" << std::endl;
				ofs << "<DataItem Reference=\"XML\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << std::endl;
				ofs << "\t/Xdmf/DataItem[@Name=\"" << *p << "_Node" << "\"]" << std::endl;
				ofs << "</DataItem>" << std::endl;
				ofs << "</Attribute>" << std::endl;
			}
			std::vector<std::string> eprops = m_mesh->get_elementdata_names();
			for (auto p=eprops.begin(); p!=eprops.end(); p++){
				ofs << "<Attribute Name=\"" << *p << "\" AttributeType=\"Scalar\" Center=\"Cell\">" << std::endl;
				ofs << "<DataItem Reference=\"XML\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements;\">" << std::endl;
				ofs << "\t/Xdmf/DataItem[@Name=\"" << *p << "_Cell" << "\"]" << std::endl;
				ofs << "</DataItem>" << std::endl;
				ofs << "</Attribute>" << std::endl;
			}
			for (auto f=0; f<m_nodefields.size(); f++){
				ofs << "<Attribute Name=\"" << m_nodefields[f] << "\" AttributeType=\"Scalar\" Center=\"Node\">" << std::endl;
				ofs << "<DataItem ItemType=\"HyperSlab\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nvertices;\">" << std::endl;
				ofs << "<DataItem Dimensions=\"3 2\" Format=\"XML\">" << std::endl;
				ofs << i << " 0" << std::endl;
				ofs << "1 1" << std::endl;
				ofs << "1 &nvertices;" << std::endl;
				ofs << "</DataItem>" << std::endl;
				write_XDMF_reference(ofs, m_nodefields[f]);
				ofs << "</DataItem>" << std::endl;
				ofs << "</Attribute>" << std::endl;
			}
			for (auto f=0; f<m_elemfields.size(); f++){
				ofs << "<Attribute Name=\"" << m_elemfields[f] << "\" AttributeType=\"Scalar\" Center=\"Cell\">" << std::endl;
				ofs << "<DataItem ItemType=\"HyperSlab\" NumberType=\"Float\" Precision=\"8\" Dimensions=\"&nelements;\">" << std::endl;
				ofs << "<DataItem Dimensions=\"3 2\" Format=\"XML\">" << std::endl;
				ofs << i << " 0" << std::endl;
				ofs << "1 1" << std::endl;
				ofs << "1 &nelements;" << std::endl;
				ofs << "</DataItem>" << std::endl;
				write_XDMF_reference(ofs, m_elemfields[f]);
				ofs << "</DataItem>" << std::endl;
				ofs << "</Attribute>" << std::endl;
			}
			ofs << "</Grid>" << std::endl;

		}
	}

	void write_XDMF_reference(std::ofstream & ofs, std::string refname){
		ofs << "<DataItem Reference=\"XML\">" << std::endl;
		ofs << "\t/Xdmf/DataItem[@Name=\"" << refname << "\"]" << std::endl;
		ofs << "</DataItem>" << std::endl;
	}

};
*/

} // end namespace simbox
#endif
