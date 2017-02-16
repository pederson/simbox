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

#include "RegularMesh.hpp"

namespace simbox{







/** @class RegularMesh3D
 *  @brief a regular mesh in 3 dimensions
 *
 *  an orthogonal mesh with a constant dx in every dimension
 *
 */

class RegularMesh3D : public RegularMesh<3>{
public:
  // constructor
  RegularMesh3D()
  : RegularMesh<3>() {};

  // copy constructor
  // RegularMesh3D(const Mesh3D & mesh);

  // inspectors

  // grid generation
  static std::shared_ptr<RegularMesh3D> generate(iNode<3> npts, Node<3> dx, Node<3> minpt){
    std::shared_ptr<RegularMesh3D> msh(new RegularMesh3D());
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
    unsigned int blf, tlf, brf, trf, blb, tlb, brb, trb;
    m_selements.resize((npts.ind[0]-1)*(npts.ind[1]-1)*(npts.ind[2]-1));
    for (auto i=0; i<npts.ind[0]-1; i++){
      for (auto j=0; j<npts.ind[1]-1; j++){
        for (auto k=0; k<npts.ind[2]-1; k++){
          glidx = element_serial_index({i,j,k});

          // blf = (nex+1)*(ney+1)*(i) + (nex+1)*(j) + k;
          blf = node_serial_index({i,j,k});
          // brf = (nex+1)*(ney+1)*(i) + (nex+1)*(j) + k+1;
          brf = node_serial_index({i,j,k+1});
          // trf = (nex+1)*(ney+1)*(i) + (nex+1)*(j+1) + k+1;
          trf = node_serial_index({i,j+1,k+1});
          // tlf = (nex+1)*(ney+1)*(i) + (nex+1)*(j+1) + k;
          tlf = node_serial_index({i,j+1,k});
          // blb = (nex+1)*(ney+1)*(i+1) + (nex+1)*(j) + k;
          blb = node_serial_index({i+1,j,k});
          // brb = (nex+1)*(ney+1)*(i+1) + (nex+1)*(j) + k+1;
          brb = node_serial_index({i+1,j,k+1});
          // trb = (nex+1)*(ney+1)*(i+1) + (nex+1)*(j+1) + k+1;
          trb = node_serial_index({i+1,j+1,k+1});
          // tlb = (nex+1)*(ney+1)*(i+1) + (nex+1)*(j+1) + k;
          tlb = node_serial_index({i+1,j+1,k});

          m_selements[glidx].nodeinds = {blf,brf,trf,tlf,blb,brb,trb,tlb};
          m_selements[glidx].type = ElementType::HEX_8;
        }
      }
    }
  }

};


}
#endif
