/** @file RegularMesh3D.hpp
 *  @brief file with 3D RegularMesh class
 *
 *  This contains the RegularMesh3D class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _REGULARMESH3D_H
#define _REGULARMESH3D_H

#include "Mesh3D.hpp"

namespace simbox{







/** @class RegularMesh3D
 *  @brief a regular mesh in 3 dimensions
 *
 *  an orthogonal mesh with a constant dx in every dimension
 *
 */

class RegularMesh3D : public Mesh3D{
public:
  // constructor
  RegularMesh3D()
  : Mesh3D() {};

  // copy constructor
  // RegularMesh3D(const Mesh3D & mesh);

  // inspectors
  // return the nodecount in a given direction
  unsigned int nodecount(unsigned int dir) const {return m_numnodes.ind[dir];};

  // return the elementcount in a given direction
  unsigned int elementcount(unsigned int dir) const {return m_numnodes.ind[dir]-1;};

  // return the dx in a given direction
  double dx(unsigned int dir) const {return m_dx.x[dir];};

  // return the index to the serialized array of nodes
  unsigned int node_serial_index(const iNode<3> & i) const {
    // uint64_t answer = 0;
    // answer |= split3(i.ind[0]) | split3(i.ind[1]) << 1 | split3(i.ind[2]) << 2;
    // return answer;
    return i.ind[2]*(m_numnodes.ind[0]*m_numnodes.ind[1]) + i.ind[1]*m_numnodes.ind[0] + i.ind[0];
  }

  // return the index to the serialized array of elements
  unsigned int element_serial_index(const iNode<3> & i) const {
    // uint64_t answer = 0;
    // answer |= split3(i.ind[0]) | split3(i.ind[1]) << 1 | split3(i.ind[2]) << 2;
    // return answer;
    return i.ind[2]*(m_numnodes.ind[0]-1)*(m_numnodes.ind[1]-1) + i.ind[1]*(m_numnodes.ind[0]-1) + i.ind[0];
  }

  // // return the array index point of a serialized node index
  // iNode<3> node_array_index(unsigned int i) const {;};

  // // return the array index point of a serialized element index
  // iNode<3> element_array_index(unsigned int i) const {;};

  // overload the nearest node operator
  unsigned int nearest_node(const Node<3> & nd) const{
    unsigned int x,y,z;
    x = (unsigned int)(double(m_numnodes.ind[0]-1)*(nd.x[0]-m_minpt.x[0])/(m_maxpt.x[0]-m_minpt.x[0]));
    y = (unsigned int)(double(m_numnodes.ind[1])*(nd.x[1]-m_minpt.x[1])/(m_maxpt.x[1]-m_minpt.x[1]));
    z = (unsigned int)(double(m_numnodes.ind[2])*(nd.x[2]-m_minpt.x[2])/(m_maxpt.x[2]-m_minpt.x[2]));
    return node_serial_index({x,y,z});
  }

  // overload the nearest element operator
  
  // grid generation
  static std::shared_ptr<RegularMesh3D> generate(iNode<3> npts, Node<3> dx, Node<3> minpt){
    std::shared_ptr<RegularMesh3D> msh(new RegularMesh3D());
    msh->generate_internal(npts, dx, minpt);
    return msh;
  }

protected:

  iNode<3>            m_numnodes;
  Node<3>             m_dx;


  // utility for Morton ordering
  // separates bits from a given integer 3 positions apart 
  uint64_t split3(unsigned int a) const {
    uint64_t x = a & 0x1fffff; // we only look at the first 21 bits
    x = (x | x << 32) & 0x1f00000000ffff;  // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
    x = (x | x << 16) & 0x1f0000ff0000ff;  // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
    x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
    x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
    x = (x | x << 2) & 0x1249249249249249;
    return x;
  }


  void generate_internal(iNode<3> npts, Node<3> dx, Node<3> minpt){
    
    // handle metadata
    m_mesh_type = REGULAR;
    m_numnodes = npts;
    m_dx = dx;
    m_minpt = minpt;
    m_maxpt = minpt + Node<3>(dx.x[0]*(npts.ind[0]-1), dx.x[1]*(npts.ind[1]-1), dx.x[2]*(npts.ind[2]-1));

    // create nodes
    m_snodes.resize(npts.ind[0]*npts.ind[1]*npts.ind[2]);
    unsigned int glidx;
    for (auto i=0; i<npts.ind[0]; i++){
      for (auto j=0; j<npts.ind[1]; j++){
        for (auto k=0; k<npts.ind[2]; k++){
          glidx = node_serial_index({i,j,k});
          m_snodes[glidx] = m_minpt + Node<3>(i*dx.x[0], j*dx.x[1], k*dx.x[2]);
        }
      }
    }

    // create elements
    m_selements.resize((npts.ind[0]-1)*(npts.ind[1]-1)*(npts.ind[2]-1));
    // for (auto i=0; i<npts.ind[0]-1; i++){
    //   for (auto j=0; j<npts.ind[1]-1; j++){
    //     for (auto k=0; k<npts.ind[2]-1; k++){
    //       glidx = node_serial_index({i,j,k});

    //       m_snodes[glidx] = m_minpt + Node<3>(i*dx.x[0], j*dx.x[1], k*dx.x[2]);
    //     }
    //   }
    // }
  }

};


}
#endif
