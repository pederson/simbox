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
	SimulationDataHDF(std::string filename, const Domain & dm, const Mesh & mesh, std::vector<double> time);
	~SimulationDataHDF();

	// inspectors
	void print_summary() const;
	const Mesh & mesh() const;
	std::vector<double> time() const;
	std::vector<std::string> nodefields() const;
	std::vector<std::string> elemfields() const;
	std::vector<std::string> transients() const;
	const double * get_nodefield(std::string fld, unsigned int tind) const;
	const double * get_nodefield(std::string fld, double t) const;
	const double * get_elemfield(std::string fld, unsigned int tind) const;
	const double * get_elemfield(std::string fld, double t) const;
	const double * get_transient(std::string tr) const;
	bool field_exists(std::string f) const;

	// mutators
	void add_elemfield(std::string fld);
	void add_nodefield(std::string fld);
	void add_transient(std::string tr);
	void set_nodefield(std::string fld, unsigned int tind, const double * data);
	void set_nodefield(std::string fld, double t, const double * data);
	void set_elemfield(std::string fld, unsigned int tind, const double * data);
	void set_elemfield(std::string fld, double t, const double * data);
	void set_transient(std::string fld, const double * data);

	// optional tasks
	void write_XDMF();
	
private:

	unsigned int get_time_index(double t);

	// domain and mesh
	const Domain * m_dm;
	const Mesh * m_mesh;

	// field std::strings
	std::vector<std::string> m_nodefields;
	std::vector<std::string> m_elemfields;
	std::vector<std::string> m_transients;

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
	void write_HDF5_mesh();
	void write_HDF5_time();

	std::vector<std::string> 		m_groupnames;
	std::map<std::string, hid_t> 	m_group_id;	// group for each field
	std::map<std::string, hid_t> 	m_dataspace_id;
	std::map<std::string, hid_t>	m_slicespace_id;
	std::map<std::string, hid_t> 	m_dataset_id;

	double * m_nodebuffer;			// buffer for node data
	double * m_elembuffer;			// buffer for element data
	double * m_transbuffer;			// buffer for transient data

	// XDMF file access
	void write_XDMF_header(std::ofstream & ofs);
	void write_XDMF_DataItems(std::ofstream & ofs);
	void write_XDMF_timesteps(std::ofstream & ofs);
	void write_XDMF_reference(std::ofstream & ofs, std::string refname);

};

#endif
