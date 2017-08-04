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
struct MeshType {};
struct Regular : public MeshType {};
struct UnstructuredTri : public MeshType {};
struct UnstructuredQuad : public MeshType {};
struct UnstructuredMixed : public MeshType {};



struct ElementType {};
struct EmptyElement : public ElementType{
  static std::string name = "Empty";
  static std::size_t numvert = 0;
};
struct Point1Element : public ElementType{
  static std::string name = "Point_1";
  static std::size_t numvert = 1;
  static std::size_t dim = 0;
};
struct Line2Element : public ElementType{
  static std::string name = "Line_2";
  static std::size_t numvert = 2;
  static std::size_t dim = 1;
};
struct Tri3Element : public ElementType{
  static std::string name = "Tri_3";
  static std::size_t numvert = 3;
  static std::size_t dim = 2;
};
struct Quad4Element : public ElementType{
  static std::string name = "Quad_4";
  static std::size_t numvert = 4;
  static std::size_t dim = 2;
};
struct Tet4Element : public ElementType{
  static std::string name = "Tet_4";
  static std::size_t numvert = 4;
  static std::size_t dim = 3;
};
struct Hex8Element : public ElementType{
  static std::string name = "Hex_8";
  static std::size_t numvert = 8;
  static std::size_t dim = 3;
};
struct Prism6Element : public ElementType{
  static std::string name = "Prism_6";
  static std::size_t numvert = 6;
  static std::size_t dim = 3;
};
struct Pyramid5Element : public ElementType{
  static std::string name = "Pyramid_5";
  static std::size_t numvert = 5;
  static std::size_t dim = 3;
};




/** @class Node
 *  @brief a single node in a mesh
 *
 *  a node in the mesh. Might or might not contain
 *  data on the node. 
 *
 */
 /*
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
*/








/** @class Element
 *  @brief a single element in a mesh
 *
 *  an element that consists of some points connected 
 *  by edges. 
 *  See http://www.manpagez.com/info/gmsh/gmsh-2.2.6/gmsh_63.php
 *  for definitions of different types
 *
 */
 /*
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
*/







/** @class Mesh
 *  @brief a mesh 
 *
 *  a bunch of points connected by edges. Or
 *  from a different perspective, a bunch of 
 *  elements (cells) with shared edges
 *
 */
template <typename MeshT, typename ElementT, 
          class NodePolicy,
          class ElementPolicy,
          class EdgePolicy,
          class FacePolicy>
class Mesh : public NodePolicy, 
             public ElementPolicy, 
             public EdgePolicy, 
             public FacePolicy
{
public:

  template <std::size_t dim,
            typename KeyT>
  const double & x(KeyT nodekey){
    return NodePolicy::x<dim>(nodekey);
  }


  template <typename ElT>
  ElT & element(KeyT elemkey){
    return ElementPolicy::operator[](elemkey);
  }

};



} // end namespace simbox
#endif
