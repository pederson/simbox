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

namespace simbox{


// these enums follow closely to the MSH format
// http://www.manpagez.com/info/gmsh/gmsh-2.2.6/gmsh_63.php
struct MeshType {};
struct Regular : public MeshType {};
struct Unstructured : public MeshType {};


struct ElementType {};
struct Point1Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 1;
  static const std::size_t dim = 0;
};
const std::string Point1Element::name = "Point_1";
struct Line2Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 2;
  static const std::size_t dim = 1;
};
const std::string Line2Element::name = "Line_2";
struct Tri3Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 3;
  static const std::size_t dim = 2;
};
const std::string Tri3Element::name = "Tri_3";
struct Quad4Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 4;
  static const std::size_t dim = 2;
};
const std::string Quad4Element::name = "Quad_4";
struct Tet4Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 4;
  static const std::size_t dim = 3;
};
const std::string Tet4Element::name = "Tet_4";
struct Hex8Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 8;
  static const std::size_t dim = 3;
};
const std::string Hex8Element::name = "Hex_8";
struct Prism6Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 6;
  static const std::size_t dim = 3;
};
const std::string Prism6Element::name = "Prism_6";
struct Pyramid5Element : public ElementType{
  static const std::string name;
  static const std::size_t numvert = 5;
  static const std::size_t dim = 3;
};
const std::string Pyramid5Element::name = "Pyramid_5";





struct DataType {};
struct NodeData : public DataType {};
struct CellData : public DataType {};
struct EdgeData : public DataType {};
struct FaceData : public DataType {};



struct NoAdapt {};


/** @class Mesh
 *  @brief a mesh 
 *
 *  At its most fundamental level, this is a container of elements. 
 *  The elements can be points, lines, tris, quads, etc...
 *
 *  Metadata is stored as template arguments MeshT, ElementT, DataLocation.
 *  
 *  NodePolicy -  handles the retrieval of node locations (x<0>,x<1>,x<2>
 *                coordinates) which could be stored explicitly in the container, or 
 *                given implicitly by a key that corresponds to container elements. Also
 *                there could be multiple nodes per element
 *
 *  DataContainer - underlying container for the stored elements
 *                  
 *  AdaptivePolicy - handles the bookkeeping associated with adaptive mesh refinement
 *
 *
 */
template <typename MeshT, typename ElementT, typename DataLocation,
          class NodePolicy,
          class DataContainer,
          class AdaptivePolicy = NoAdapt>
class Mesh : public NodePolicy, public DataContainer, public AdaptivePolicy
{
public:
  static_assert(std::is_base_of<MeshType, MeshT>::value, "MeshT must be derived from simbox::MeshType");
  static_assert(std::is_base_of<ElementType, ElementT>::value, "ElementT must be derived from simbox::ElementType");
  static_assert(std::is_base_of<DataType, DataLocation>::value, "DataLocation must be derived from simbox::DataType");

  // provide a constructor that is identical to the underlying DataContainer

  // total number of nodes (including repeats)
  std::size_t nodecount() const {return ElementT::numvert*DataContainer::size();};

  // total number of elements
  std::size_t elementcount() const {return DataContainer::size();};
};



} // end namespace simbox
#endif
