/** @file Mesh.hpp
 *  @brief file with generic Mesh class
 *
 *  This contains the base Mesh class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _MESH_H
#define _MESH_H

#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>

// #include "mpitools.hpp"

namespace simbox{

// these enums follow closely to the MSH format
// http://www.manpagez.com/info/gmsh/gmsh-2.2.6/gmsh_63.php
enum MeshType{REGULAR=0, UNSTRUCTURED_TRI=1, UNSTRUCTURED_QUAD=2, UNSTRUCTURED_MIXED=3, MESH_UNKNOWN};
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
    if (dim > 3){
      std::cerr << "Node: That constructor not implemented for dim > 3" << std::endl;
      throw -1;
    }
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






/** @class Node
 *  @brief a single node in a mesh
 *
 *  a node in the mesh. Might or might not contain
 *  data on the node. 
 *
 */
template<std::size_t dim>
class iNode{
public:
  // data
  unsigned int          ind[dim];


  // constructor
  iNode(unsigned int i0=0.0, unsigned int i1=0.0, unsigned int i2=0.0){
    ind[0] = i0;
    if (dim > 1) ind[1] = i1;
    if (dim > 2) ind[2] = i2;
    if (dim > 3){
      std::cerr << "iNode: That constructor not implemented for dim > 3" << std::endl;
      throw -1;
    }
  }
  
  // constructor
  iNode(std::vector<unsigned int> indi){
    for (auto i=0; i<indi.size(); i++) ind[i] = indi[i];
  }

  // copy constructor
  iNode(const iNode & p){
    for (auto i=0; i<dim; i++) ind[i] = p.ind[i];
  }

  // assignment
  iNode & operator= (const iNode & p){
    for (auto i=0; i<dim; i++) ind[i] = p.ind[i];
    return *this;
  }

  // addition
  iNode operator+ (const iNode & p) const{
    iNode out(p);
    for (auto i=0; i<dim; i++) out.ind[i] = ind[i] + p.ind[i];
    return out;
  }

  // subtraction
  iNode operator- (const iNode & p) const{
    iNode out(p);
    for (auto i=0; i<dim; i++) out.ind[i] = ind[i] - p.ind[i];
    return out;
  }

  // scalar multiplication
  iNode operator* (double val) const{
    iNode out(*this);
    for (auto i=0; i<dim; i++) out.ind[i] = val*ind[i];
    return out;
  }

  // comparison
  bool operator== (const iNode & p) const {
    for (auto i=0; i<dim; i++) if (ind[i] != p.ind[i]) return false;
    return true;
  }

  static double dist(const iNode & p1, const iNode & p2){
    double dsq = 0.0;
    for (auto i=0; i<dim; i++) dsq += (p1.ind[i] - p2.ind[i])*(p1.ind[i] - p2.ind[i]);
    return sqrt(dsq);
  }

  // 1 norm
  static double dist1(const iNode & p1, const iNode & p2){
    double dsq = 0.0;
    for (auto i=0; i<dim; i++) dsq += fabs(p1.ind[i] - p2.ind[i]);
    return dsq;
  }

  static double distsq(const iNode & p1, const iNode & p2){
    double dsq = 0.0;
    for (auto i=0; i<dim; i++) dsq += (p1.ind[i] - p2.ind[i])*(p1.ind[i] - p2.ind[i]);
    return dsq;
  }

  static double dot(const iNode & p1, const iNode & p2){
    double dt = 0.0;
    for (auto i=0; i<dim; i++) dt += p1.ind[i]*p2.ind[i];
    return dt;
  }

  // print to std::out
  template<std::size_t d>
  friend std::ostream & operator<<(std::ostream & os, const iNode<d> & p);

};

template<std::size_t dim>
std::ostream & operator<<(std::ostream & os, const iNode<dim> & p){
  os << "(" ;
  for (auto i=0; i< dim-1; i++) os << p.ind[i] << ", " ;
  os << p.ind[dim-1] << ")" ;
  
  return os;
}

template<std::size_t dim>
iNode<dim> operator*(double val, const iNode<dim> & p){
  iNode<dim> out(p);
  for (auto i=0; i<dim; i++) out.ind[i] = val*p.ind[i];
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
  
  // constructor
  Element()
  : type(ElementType::EMPTY_0) {};

  // constructor
  Element(std::vector<unsigned int> node_inds, ElementType etype)
  : nodeinds(), type(etype) {};

  // assignment
  Element & operator= (const Element & p){
    nodeinds = p.nodeinds;
    type = p.type;
    return *this;
  }

  // utils
  void print_summary(std::ostream & os=std::cout) const{
    os << "<Element topology=\"" << get_string(type) << "\">";
    for (unsigned int i=0; i<nodeinds.size(); i++) os << nodeinds.at(i) << " ";
    os << "</Element>" ;
    return;
  }
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
  // constructor
  Mesh()
  : m_mesh_type(MESH_UNKNOWN) {};

  // copy constructor
  // Mesh(const Mesh & mesh);
  
  // // destructor
  // ~Mesh();

  void print_summary(std::ostream & os=std::cout) const{
    os << "<Mesh " ;
    os << "dim=\"" << dim << "\"" ;
    
    if (snodecount() == 0){
      os << ">" << std::endl;
      os << "</Mesh>" << std::endl;
      return;
    }

    

    os << " type=\"";
    if (m_mesh_type==REGULAR) os << "REGULAR" ;
    else if (m_mesh_type==UNSTRUCTURED_TRI) os << "UNSTRUCTURED_TRI" ;
    else if (m_mesh_type==UNSTRUCTURED_QUAD) os << "UNSTRUCTURED_QUAD" ;
    else if (m_mesh_type == UNSTRUCTURED_MIXED) os << "UNSTRUCTURED_MIXED" ;
    else os << "MESH_UNKNOWN" ;
    os << "\"" ;
    os << ">" << std::endl;

    os << "\t<snodecount>" << snodecount() << "</snodecount>" << std::endl;
    // os << "\t<sedgecount>" << sedgecount() << "</sedgecount>" << std::endl;
    os << "\t<selementcount>" << selementcount() << "</selementcount>" << std::endl;
    os << "\t<dnodecount>" << dnodecount() << "</dnodecount>" << std::endl;
    // os << "\t<dedgecount>" << dedgecount() << "</dedgecount>" << std::endl;
    os << "\t<delementcount>" << delementcount() << "</delementcount>" << std::endl;

    os << "\t<Extents>" << m_minpt << ", " << m_maxpt << "</Extents>" << std::endl;
    
    for (auto i=m_nodedata.begin(); i!=m_nodedata.end(); i++) os << "\n" << "\t<NodeData>" << i->first << "</NodeData>" << std::endl;
    for (auto i=m_elementdata.begin(); i!=m_elementdata.end(); i++) os << "\n" << "\t<ElementData>" << i->first << "</ElementData>" << std::endl;

    os << "</Mesh>" << std::endl;

    return;
  }

  // void print_detailed(std::ostream & os=std::cout) const;
  

  // metadata access
  MeshType mesh_type() const {return m_mesh_type;};
  unsigned int snodecount() const {return m_snodes.size();};
  // unsigned int sedgecount() const {return m_sedges.size();};
  unsigned int selementcount() const {return m_selements.size();};
  unsigned int dnodecount() const {return m_dnodes.size();};
  // unsigned int dedgecount() const {return m_dedges.size();};
  unsigned int delementcount() const {return m_delements.size();};

  unsigned int nearest_node(const Node<dim> & n) const{
    double dsq = Node<dim>::distsq(n,m_snodes[0]);
    unsigned int minloc = 0;
    double minval = dsq;
    for (auto i=1; i<m_snodes.size(); i++){
      dsq = Node<dim>::distsq(n,m_snodes[i]);
      if (dsq < minval){
        minloc = i;
        minval = dsq;
      }  
    }
    return minloc;
  }
  // unsigned int nearest_element(const Node & n) const;

  // node and element access
  Node<dim> & snode(unsigned int i) {return m_snodes.at(i);};
  // Edge & sedge(unsigned int i) {return m_sedge.at(i);};
  Element<dim> & selement(unsigned int i) {return m_selements.at(i);};
  const Node<dim> & snode(unsigned int i) const {return m_snodes.at(i);};
  // const Edge & sedge(unsigned int i) const {return m_sedge.at(i);};
  const Element<dim> & selement(unsigned int i) const {return m_selements.at(i);};

  Node<dim> & dnode(unsigned int i) {return m_dnodes.at(i);};
  // Edge & dedge(unsigned int i) {return m_dedge.at(i);};
  Element<dim> & delement(unsigned int i) {return m_delements.at(i);};
  const Node<dim> & dnode(unsigned int i) const {return m_dnodes.at(i);};
  // const Edge & dedge(unsigned int i) const {return m_dedge.at(i);};
  const Element<dim> & delement(unsigned int i) const {return m_delements.at(i);};

  // property interaction and access
  const std::vector<double> & nodedata(std::string fieldname) const {return m_nodedata.at(fieldname);};
  const std::vector<double> & elementdata(std::string fieldname) const {return m_elementdata.at(fieldname);};

  // void set_nodecount(unsigned int count);
  // void set_elementcount(unsigned int count);

  void add_nodedata(std::string property_name, const double * values){
    std::vector<double> prop;
    prop.assign(m_snodes.size(), 0.0);
    for (unsigned int i=0; i<m_snodes.size(); i++) prop.at(i) = values[i];
    m_nodedata[property_name] = prop;
  }

  void add_nodedata(std::string property_name, double init_val){
    std::vector<double> prop;
    prop.assign(m_snodes.size(), init_val);
    m_nodedata[property_name] = prop;
  }

  void add_elementdata(std::string property_name, const double * values){
    std::vector<double> prop;
    prop.assign(m_selements.size(), 0.0);
    for (unsigned int i=0; i<m_selements.size(); i++) prop.at(i) = values[i];
    m_elementdata[property_name] = prop;
  }

  void add_elementdata(std::string property_name, double init_val){
    std::vector<double> prop;
    prop.assign(m_selements.size(), init_val);
    m_elementdata[property_name] = prop;
  }

  void set_nodedata(std::string property_name, unsigned int i, double val){m_nodedata.at(property_name).at(i) = val;};

  void set_elementdata(std::string property_name, unsigned int i, double val){m_elementdata.at(property_name).at(i) = val;};
  
  std::vector<std::string> get_nodedata_names() const{
    std::vector<std::string> names;
    for (auto p=m_nodedata.begin(); p!=m_nodedata.end(); p++){
      names.push_back(p->first);
    }
    return names;
  }
  
  std::vector<std::string> get_elementdata_names() const{
    std::vector<std::string> names;
    for (auto p=m_elementdata.begin(); p!=m_elementdata.end(); p++){
      names.push_back(p->first);
    }
    return names;
  }

  void calc_extents(){
    m_minpt = m_snodes.at(0);
    m_maxpt = m_snodes.at(0);
    for (unsigned int i=1; i<m_snodes.size(); i++){
      // std::cout << "I: " << i << m_snodes.at(i) << std::endl;
      for (auto j=0; j<dim; j++){
        if (m_snodes.at(i).x[j] < m_minpt.x[j]) m_minpt.x[j] = m_snodes.at(i).x[j];
        if (m_snodes.at(i).x[j] > m_maxpt.x[j]) m_maxpt.x[j] = m_snodes.at(i).x[j];
      }
    }
    // std::cout << "minpt: " << m_minpt << " maxpt: " << m_maxpt << std::endl;
    return;
  }

protected:
  // metadata
  MeshType                        m_mesh_type;
  Node<dim>                       m_minpt, m_maxpt;

  // nodes and elements
  std::vector<Node<dim>>          m_snodes;     // array of STATIC nodes
  // std::vector<Edge<dim>>          m_sedges;     // array of STATIC edges
  std::vector<Element<dim>>       m_selements;  // array of STATIC elements
  std::list<Node<dim>>            m_dnodes;     // array of DYNAMIC nodes
  // std::list<Edge<dim>>            m_dedges;     // array of DYNAMIC edges
  std::list<Element<dim>>         m_delements;  // array of DYNAMIC elements

  // user-defined properties for the mesh
  std::map<std::string, std::vector<double>>      m_nodedata;
  std::map<std::string, std::vector<double>>      m_elementdata;

};

}
#endif
