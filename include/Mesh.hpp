/** @file Mesh.hpp
 *  @brief file with generic Mesh class
 *
 *  This contains the base Mesh class
 *  and associated functions, defs, and enums
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _MESH_H
#define _MESH_H

#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>

#include "mpitools.hpp"

namespace simbox{

// these enums follow closely to the MSH format
// http://www.manpagez.com/info/gmsh/gmsh-2.2.6/gmsh_63.php
enum MeshType{REGULAR=0, UNSTRUCTURED_TRI=1, UNSTRUCTURED_QUAD=2, MESH_UNKNOWN};
enum class ElementType : unsigned int {EMPTY_0=0, POINT_1, LINE_2, TRI_3, QUAD_4, TET_4, HEX_8, PRISM_6, PYRAMID_5, UNKNOWN};

const std::vector<std::string> elnames = {"Empty", "Point_1", "Line_2", "Tri_3", "Quad_4", "Tet_4", "Hex_8", "Prism_6", "Pyramid_5", "Unknown"};
const std::vector<unsigned int> nvert = {0, 1, 2, 3, 4, 4, 8, 6, 5, 0};
inline std::string get_string(ElementType et){return elnames[(int)et];};
// inline ElementType get_element(std::string els){
//   for (auto e=ElementType::EMPTY_0; e<ElementType::UNKNOWN; e++){
//     if (get_string(e).compare(els)==0) return e;
//   }
//   return ElementType::UNKNOWN;
// }
inline unsigned int get_nvert(ElementType et){return nvert[(int)et];};




/** @class Node
 *  @brief a single node in a mesh
 *
 *  a node in the mesh. Might or might not contain
 *  data on the node. 
 *
 */
template<std::size_t dim>
class Node{
public:
  // data
  double          x[dim];

  // constructor
  Node(double x0=0.0, double x1=0.0, double x2=0.0){
    x[0] = x0;
    if (dim > 1) x[1] = x1;
    if (dim > 2) x[2] = x2;
    if (dim > 3) throw("ERROR: That Node constructor not implemented for dim > 3");
  }
  
  // constructor
  Node(std::vector<double> xin){
    for (auto i=0; i<xin.size(); i++) x[i] = xin[i];
  }

  // copy constructor
  Node(const Node & p){
    for (auto i=0; i<dim; i++) x[i] = p.x[i];
  }

  // assignment
  Node & operator= (const Node & p){
    for (auto i=0; i<dim; i++) x[i] = p.x[i];
    return *this;
  }

  // addition
  Node operator+ (const Node & p) const{
    Node out(p);
    for (auto i=0; i<dim; i++) out.x[i] = x[i] + p.x[i];
    return out;
  }

  // subtraction
  Node operator- (const Node & p) const{
    Node out(p);
    for (auto i=0; i<dim; i++) out.x[i] = x[i] - p.x[i];
    return out;
  }

  // scalar multiplication
  Node operator* (double val) const{
    Node out(*this);
    for (auto i=0; i<dim; i++) out.x[i] = val*x[i];
    return out;
  }

  // comparison
  bool operator== (const Node & p) const {
    for (auto i=0; i<dim; i++) if (x[i] != p.x[i]) return false;
    return true;
  }

  static double dist(const Node & p1, const Node & p2){
    double dsq = 0.0;
    for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
    return sqrt(dsq);
  }

  static double distsq(const Node & p1, const Node & p2){
    double dsq = 0.0;
    for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
    return dsq;
  }

  static double dot(const Node & p1, const Node & p2){
    double dt = 0.0;
    for (auto i=0; i<dim; i++) dt += p1.x[i]*p2.x[i];
    return dt;
  }

  // print to std::out
  template<std::size_t d>
  friend std::ostream & operator<<(std::ostream & os, const Node<d> & p);

};

template<std::size_t dim>
std::ostream & operator<<(std::ostream & os, const Node<dim> & p){
  os << "(" ;
  for (auto i=0; i< dim-1; i++) os << p.x[i] << ", " ;
  os << p.x[dim-1] << ")" ;
  
  return os;
}

template<std::size_t dim>
Node<dim> operator*(double val, const Node<dim> & p){
  Node<dim> out(p);
  for (auto i=0; i<dim; i++) out.x[i] = val*p.x[i];
  return out;
}




/** @class Element
 *  @brief a single element in a mesh
 *
 *  an element that consists of some points connected 
 *  by edges. 
 *  See http://www.manpagez.com/info/gmsh/gmsh-2.2.6/gmsh_63.php
 *  for definitions of different types
 *
 */
template<std::size_t dim>
class Element{
public:
  // data
  std::vector<unsigned int>           nodeinds;
  ElementType                         type;
  

  Element();
  Element(std::vector<unsigned int> node_inds);
  ~Element();

  // utils
  void print_summary(std::ostream & os=std::cout) const;

private:

  void recalc_type();

};




/** @class Mesh
 *  @brief a mesh in 1, 2, or 3 dimensions
 *
 *  a bunch of points connected by edges. Or
 *  from a different perspective, a bunch of 
 *  elements (cells) with shared edges
 *
 */
template<std::size_t dim>
class Mesh{
public:
  Mesh();   // constructor
  Mesh(const Mesh & mesh);  // copy constructor
  ~Mesh();    // destructor

  void print_summary(std::ostream & os=std::cout) const;
  void print_detailed(std::ostream & os=std::cout) const;
  

  // metadata access
  MeshType mesh_type() const {return _mesh_type;};
  unsigned int nodecount() const {return m_snodes.size()+m_dnodes.size();};
  unsigned int edgecount() const {return m_sedges.size()+m_dedges.size();};
  unsigned int elementcount() const {return m_selements.size()+m_delements.size();};
  // double xmin() const {return _xmin;};
  // double ymin() const {return _ymin;};
  // double zmin() const {return _zmin;};
  // double xmax() const {return _xmax;};
  // double ymax() const {return _ymax;};
  // double zmax() const {return _zmax;};
  unsigned int nearest_node(const Node & n) const;
  unsigned int nearest_element(const Node & n) const;

  // node and element access
  Node & snode(unsigned int i) {return m_snodes.at(i);};
  Edge & sedge(unsigned int i) {return m_sedge.at(i);};
  Element & selement(unsigned int i) {return m_selements.at(i);};
  const Node & snode(unsigned int i) const {return m_snodes.at(i);};
  const Edge & sedge(unsigned int i) const {return m_sedge.at(i);};
  const Element & selement(unsigned int i) const {return m_selements.at(i);};
  
  double get_element_center_x(unsigned int i) const;
  double get_element_center_y(unsigned int i) const;
  double get_element_center_z(unsigned int i) const;

  // property interaction and access
  const double & x();
  const double & y();
  const double & z();
  const bool & boundary();
  const unsigned int & core_group();
  const unsigned int & num_connections();
  const double & data(std::string fieldname) const;

  const double * nodedata(std::string fieldname) const;
  const double * elementdata(std::string fieldname) const;

  void set_nodecount(unsigned int count);
  void set_elementcount(unsigned int count);
  void set_num_dims(unsigned int ndims) {_num_dims = ndims;};

  void add_phys_property(std::string property_name, const double * property_vals);
  void add_phys_property(std::string proprety_name, double init_val);
  std::vector<std::string> get_phys_properties() const {return _phys_property_names;};

  void add_nodedata(std::string property_name, const double * values);
  void add_nodedata(std::string proprety_name, double init_val);
  void add_elementdata(std::string property_name, const double * values);
  void add_elementdata(std::string proprety_name, double init_val);
  void set_nodedata(std::string property_name, unsigned int i, double val){m_nodedata.at(property_name).at(i) = val;};
  void set_elementdata(std::string property_name, unsigned int i, double val){m_elementdata.at(property_name).at(i) = val;};
  std::vector<std::string> get_nodedata_names() const;
  std::vector<std::string> get_elementdata_names() const;

  void reset_property(std::string property_name, double reset_val=0.0);
  void set_phys_property(std::string property_name, unsigned int i, double val){_phys_properties.at(property_name).at(i) = val;};
  void increment_phys_property(std::string property_name, unsigned int i, double val){_phys_properties.at(property_name).at(i) += val;};

  // grid generation and refinement
  //static Mesh create_unstructured_tri_simple();

  // reading and writing files
  static Mesh read_MSH(std::string filename);
  static Mesh read_XML(std::string filename);
  static Mesh read_NEU(std::string filename, unsigned int byte_offset=0);
  static Mesh read_CAS(std::string filename, unsigned int byte_offset=0);
  void write_MSH(std::string filename) const;
  void write_NEU(std::string filename) const;
  void write_CAS(std::string filename) const;

  void calc_extents();

//private:
protected:
  // metadata
  MeshType m_mesh_type;
  Box<dim> m_bounds;
  // double _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;
  

  // nodes and elements
  std::vector<Node>               m_snodes;     // array of STATIC nodes
  std::vector<Edge>               m_sedges;     // array of STATIC edges
  std::vector<Element>            m_selements;  // array of STATIC elements
  std::vector<Node>               m_dnodes;     // array of DYNAMIC nodes
  std::vector<Edge>               m_dedges;     // array of DYNAMIC edges
  std::vector<Element>            m_delements;  // array of DYNAMIC elements

  // user-defined properties for the mesh
  // DYLAN_TODO: RENAME THIS NodeData and ElementData
  std::vector<std::string> _phys_property_names; // the name position in this vector corresponds with the position of the property value in the node
  std::map<std::string, std::vector<double>> _phys_properties;
  
  std::map<std::string, std::vector<double>> m_nodedata;
  std::map<std::string, std::vector<double>> m_elementdata;

  // other properties conveniently placed in arrays (on demand) in order to return data
  std::vector<bool> _boundary;
  std::vector<double> _x, _y, _z;
  std::vector<unsigned int> _core_group, _num_connections;

  void read_MSH_internal(std::string filename);

  

};




}
#endif
