/** @file SimulationDataHDF.hpp
 *  @brief file with SimulationDataHDF class
 *
 *  This contains the SimulationDataHDF class
 *  class and associated functions, defs, and enums
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _SIMULATIONDATAHDF_H
#define _SIMULATIONDATAHDF_H

#include "SimulationData.hpp"
#include "mpitools.hpp"
#include <hdf5.h>


using namespace std;

/** @class SimulationDataHDF
 *  @brief class for I/O using HDF5 file format
 *
 *  Use this class to access and write data to/from
 *	the HDF5 file format. Parallel read/write is 
 *	possible with the HDF5 format using MPI
 *
 */
class SimulationDataHDF : public SimulationData{
public:
	SimulationDataHDF(string filename, const Domain & dm, const Mesh & mesh, vector<double> time);
	~SimulationDataHDF();

	// inspectors
	void print_summary() const;
	const Mesh & mesh() const;
	vector<double> time() const;
	vector<string> nodefields() const;
	vector<string> cellfields() const;
	vector<string> transients() const;
	const double * get_nodefield(string fld, unsigned int tind) const;
	const double * get_nodefield(string fld, double t) const;
	const double * get_cellfield(string fld, unsigned int tind) const;
	const double * get_cellfield(string fld, double t) const;
	const double * get_transient(string tr) const;
	bool field_exists(string f) const;

	// mutators
	void add_cellfield(string fld);
	void add_nodefield(string fld);
	void add_transient(string tr);
	void set_nodefield(string fld, unsigned int tind, const double * data);
	void set_nodefield(string fld, double t, const double * data);
	void set_cellfield(string fld, unsigned int tind, const double * data);
	void set_cellfield(string fld, double t, const double * data);
	void set_transient(string fld, const double * data);

	// optional tasks
	void write_XDMF();
	
private:

	unsigned int get_time_index(double t);

	// domain and mesh
	const Domain * m_dm;
	const Mesh * m_mesh;

	// field strings
	vector<string> m_nodefields;
	vector<string> m_cellfields;
	vector<string> m_transients;

	// time info
	vector<double> m_time;

	// HDF5 file resources
	string 		m_filename;
	hid_t 		m_h5file;
	hid_t		m_plist_id;
	hid_t		m_transient_space;
	hid_t		m_nodefield_space;
	hid_t		m_cellfield_space;
	hid_t		m_node_memspace;
	hid_t		m_cell_memspace;

	// HDF5 file access
	void write_HDF5_mesh();
	void write_HDF5_time();

	vector<string> 		m_groupnames;
	map<string, hid_t> 	m_group_id;	// group for each field
	map<string, hid_t> 	m_dataspace_id;
	map<string, hid_t>	m_slicespace_id;
	map<string, hid_t> 	m_dataset_id;

	double * m_nodebuffer;
	double * m_cellbuffer;
	double * m_transbuffer;

	// XDMF file access
	void write_XDMF_header(std::ofstream & ofs);
	void write_XDMF_DataItems(std::ofstream & ofs);
	void write_XDMF_timesteps(std::ofstream & ofs);
	void write_XDMF_reference(std::ofstream & ofs, string refname);

};

#endif
