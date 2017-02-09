/** @file SimulationData.hpp
 *  @brief file with SimulationData base
 *
 *  This contains the SimulationData base
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _SIMULATIONDATA_H
#define _SIMULATIONDATA_H

#include "Domain.hpp"
#include "Mesh.hpp"

#include <string>
#include <map>
#include <vector>

 namespace simbox{

// topology names corresponding to ElementType from Mesh.hpp
const std::vector<std::string> xdmf_topology = {"Empty","Polyvertex","Polyline",
									  "Triangle", "Quadrilateral","Tetrahedron",
									  "Hexahedron","Unknown","Pyramid","Unknown"};

/** @class SimulationData
 *  @brief base class for simulationd data I/O
 *
 *  Simulation data base class that defines standard
 *	functions for I/O involving simulations
 *
 */
template<std::size_t dim>
class SimulationData{
public:

	// inspectors
	virtual void print_summary(std::ostream & os = std::cout) const = 0;
	virtual const Mesh<dim> & mesh() const = 0;
	virtual std::vector<double> time() const = 0;
	virtual std::vector<std::string> nodefields() const = 0;
	virtual std::vector<std::string> elemfields() const = 0;
	virtual std::vector<std::string> transients() const = 0;
	virtual const double * get_nodefield(std::string fld, unsigned int tind) const = 0;
	virtual const double * get_nodefield(std::string fld, double t) const = 0;
	virtual const double * get_elemfield(std::string fld, unsigned int tind) const = 0;
	virtual const double * get_elemfield(std::string fld, double t) const = 0;
	virtual const double * get_transient(std::string tr) const = 0;
	virtual bool field_exists(std::string f) const = 0;

	// mutators
	virtual void add_elemfield(std::string fld) = 0;
	virtual void add_nodefield(std::string fld) = 0;
	virtual void add_transient(std::string tr) = 0;
	virtual void set_nodefield(std::string fld, unsigned int tind, const double * data) = 0;
	virtual void set_nodefield(std::string fld, double t, const double * data) = 0;
	virtual void set_elemfield(std::string fld, unsigned int tind, const double * data) = 0;
	virtual void set_elemfield(std::string fld, double t, const double * data) = 0;
	virtual void set_transient(std::string fld, const double * data) = 0;

	// optional tasks
	virtual void write_XDMF() = 0;
};

}
#endif