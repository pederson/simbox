/** @file RegularMesh2D.hpp
 *  @brief file with 2D RegularMesh class
 *
 *  This contains the RegularMesh2D class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _REGULARMESH2D_H
#define _REGULARMESH2D_H

#include "RegularMesh.hpp"

namespace simbox{





/** @class RegularMesh2D
 *  @brief a regular mesh in 2 dimensions
 *
 *  an orthogonal mesh with a constant dx in every dimension
 *
 */

class RegularMesh2D : public RegularMesh<2>{
public:
  // constructor
  RegularMesh2D()
  : RegularMesh<2>() {};

  // copy constructor
  // RegularMesh2D(const Mesh2D & mesh);

  // inspectors
  
  // grid generation
  static std::shared_ptr<RegularMesh2D> generate(iNode<2> npts, Node<2> dx, Node<2> minpt){
    std::shared_ptr<RegularMesh2D> msh(new RegularMesh2D());
    msh->generate_internal(npts, dx, minpt);
    return msh;
  }

protected:


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


  void generate_internal(iNode<2> npts, Node<2> dx, Node<2> minpt){
    
    // handle metadata
    m_mesh_type = REGULAR;
    m_numnodes = npts;
    m_dx = dx;
    m_minpt = minpt;
    m_maxpt = minpt + Node<2>(dx.x[0]*(npts.ind[0]-1), dx.x[1]*(npts.ind[1]-1));

    // create nodes
    m_snodes.resize(npts.ind[0]*npts.ind[1]);
    unsigned int glidx;
    for (auto i=0; i<npts.ind[0]; i++){
      for (auto j=0; j<npts.ind[1]; j++){
        glidx = node_serial_index({i,j});
        m_snodes[glidx] = m_minpt + Node<2>(i*dx.x[0], j*dx.x[1]);
      }
    }

    // create elements
    unsigned int bl, tl, br, tr;
    m_selements.resize((npts.ind[0]-1)*(npts.ind[1]-1));
    for (auto i=0; i<npts.ind[0]-1; i++){
      for (auto j=0; j<npts.ind[1]-1; j++){
        glidx = element_serial_index({i,j});

        bl = node_serial_index({i,j});
        br = node_serial_index({i+1,j});
        tr = node_serial_index({i+1,j+1});
        tl = node_serial_index({i,j+1});

        m_selements[glidx].nodeinds = {bl,br,tr,tl};
        m_selements[glidx].type = ElementType::QUAD_4;
      }
    }
  }

};


}
#endif
