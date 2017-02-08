/** @file Mesh2D.hpp
 *  @brief file with 3D Mesh class
 *
 *  This contains the base Mesh2D class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _MESH2D_H
#define _MESH2D_H

#include "Mesh.hpp"
#include "Mesh3D.hpp"

namespace simbox{




/** @class Mesh2D
 *  @brief a mesh in 3 dimensions
 *
 *  a bunch of points connected by edges. Or
 *  from a different perspective, a bunch of 
 *  elements (cells) with shared edges
 *
 */

class Mesh2D : public Mesh<2>{
public:
  // constructor
  Mesh2D()
  : Mesh<2>() {};

  // copy constructor
  // Mesh2D(const Mesh2D & mesh);

  // constructor by 3D mesh
  Mesh2D(const Mesh3D & mesh);
  
  // destructor
  ~Mesh2D();


  // grid generation and refinement
  //static Mesh create_unstructured_tri_simple();

  // reading and writing files
  static std::shared_ptr<Mesh2D> read_MSH(std::string filename);
  // static Mesh read_XML(std::string filename);
  // static Mesh read_NEU(std::string filename, unsigned int byte_offset=0);
  // static Mesh read_CAS(std::string filename, unsigned int byte_offset=0);
  // void write_MSH(std::string filename) const;
  // void write_NEU(std::string filename) const;
  // void write_CAS(std::string filename) const;

protected:



};


std::shared_ptr<Mesh2D> Mesh2D::read_MSH(std::string filename){
  // read MSH file with 3D reader
  std::shared_ptr<Mesh3D> mesh3 = Mesh3D::read_MSH(filename);

  // convert to 2D mesh
  std::shared_ptr<Mesh2D> outmesh(*mesh3);
  return outmesh;
}



}
#endif
