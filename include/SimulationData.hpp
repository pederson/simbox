/** @file SimulationData.hpp
 *  @brief file with SimulationData base
 *
 *  This contains the SimulationData base
 *  class and associated functions, defs, and enums
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _SIMULATIONDATA_H
#define _SIMULATIONDATA_H

#include "Domain.hpp"
#include "Mesh.hpp"

#include <string>
#include <map>
#include <vector>

using namespace std;

// topology names corresponding to ElementType from Mesh.hpp
const vector<string> xdmf_topology = {"Empty","Polyvertex","Polyline",
									  "Triangle", "Quadrilateral","Tetrahedron",
									  "Hexahedron","Unknown","Pyramid","Unknown"};

/** @class SimulationData
 *  @brief base class for simulationd data I/O
 *
 *  Simulation data base class that defines standard
 *	functions for I/O involving simulations
 *
 */
class SimulationData{
public:

	// inspectors
	virtual void print_summary() const = 0;
	virtual const Mesh & mesh() const = 0;
	virtual vector<double> time() const = 0;
	virtual vector<string> nodefields() const = 0;
	virtual vector<string> cellfields() const = 0;
	virtual vector<string> transients() const = 0;
	virtual const double * get_nodefield(string fld, unsigned int tind) const = 0;
	virtual const double * get_nodefield(string fld, double t) const = 0;
	virtual const double * get_cellfield(string fld, unsigned int tind) const = 0;
	virtual const double * get_cellfield(string fld, double t) const = 0;
	virtual const double * get_transient(string tr) const = 0;
	virtual bool field_exists(string f) const = 0;

	// mutators
	virtual void add_cellfield(string fld) = 0;
	virtual void add_nodefield(string fld) = 0;
	virtual void add_transient(string tr) = 0;
	virtual void set_nodefield(string fld, unsigned int tind, const double * data) = 0;
	virtual void set_nodefield(string fld, double t, const double * data) = 0;
	virtual void set_cellfield(string fld, unsigned int tind, const double * data) = 0;
	virtual void set_cellfield(string fld, double t, const double * data) = 0;
	virtual void set_transient(string fld, const double * data) = 0;

	// optional tasks
	virtual void write_XDMF() = 0;
};

#endif