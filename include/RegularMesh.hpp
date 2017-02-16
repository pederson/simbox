/** @file RegularMesh.hpp
 *  @brief file with RegularMesh base class
 *
 *  This contains the RegularMesh class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _REGULARMESH_H
#define _REGULARMESH_H

#include "Mesh.hpp"

namespace simbox{




/** @class RegularMesh
 *  @brief a regular mesh in arbitrary dimension
 *
 *  an orthogonal mesh with a constant dx in every dimension
 *
 */
template <std::size_t dim>
class RegularMesh : public Mesh<dim>{
public:
  // // constructor
  // RegularMesh()
  // : Mesh<dim>() {};

  // copy constructor
  // RegularMesh(const Mesh<dim> & mesh);

  // inspectors
  // return the nodecount in a given direction
  unsigned int nodecount(unsigned int dir) const {return m_numnodes.ind[dir];};

  // return the elementcount in a given direction
  unsigned int elementcount(unsigned int dir) const {return m_numnodes.ind[dir]-1;};

  // return the dx in a given direction
  double dx(unsigned int dir) const {return m_dx.x[dir];};

  // return the index to the serialized array of nodes
  unsigned int node_serial_index(const iNode<dim> & i) const {
    // uint64_t answer = 0;
    // answer |= split3(i.ind[0]) | split3(i.ind[1]) << 1 | split3(i.ind[2]) << 2;
    // return answer;
    unsigned int answer = 0;
    unsigned int nn = 1;
    for (auto j=0; j<dim; j++){
      answer += nn*i.ind[j];
      nn *= m_numnodes.ind[j];
    }
    return answer;
  }

  // return the index to the serialized array of elements
  unsigned int element_serial_index(const iNode<dim> & i) const {
    // uint64_t answer = 0;
    // answer |= split3(i.ind[0]) | split3(i.ind[1]) << 1 | split3(i.ind[2]) << 2;
    // return answer;
    unsigned int answer = 0;
    unsigned int nn = 1;
    for (auto j=0; j<dim; j++){
      answer += nn*i.ind[j];
      nn *= m_numnodes.ind[j]-1;
    }
    return answer;
  }

  // return the array index point of a serialized node index
  iNode<dim> node_array_index(unsigned int i) const {
    iNode<dim> out;
    unsigned int nn = 1;
    for (auto d=dim-1; d>=0; d--){
      out.ind[d] = i%(nn*m_numnodes.ind[d]);
      nn *= m_numnodes.ind[d];
    }
    return out;
  };

  // return the array index point of a serialized element index
  iNode<dim> element_array_index(unsigned int i) const {
    iNode<dim> out;
    unsigned int nn = 1;
    for (auto d=dim-1; d>=0; d--){
      out.ind[d] = i%(nn*(m_numnodes.ind[d]-1));
      nn *= m_numnodes.ind[d]-1;
    }
    return out;
  };


  // get neighbor node on the min side in a given direction
  unsigned int neighbor_node_min(unsigned int nind, unsigned int dir) const{
    iNode<dim> in = node_array_index(nind);
    in.ind[dir] -= 1;
    return node_serial_index(in);
  }

  // get neighbor node on the max side in a given direction
  unsigned int neighbor_node_max(unsigned int nind, unsigned int dir) const{
    iNode<dim> in = node_array_index(nind);
    in.ind[dir] += 1;
    return node_serial_index(in);
  }

  // get neighbor element on the min side in a given direction
  unsigned int neighbor_element_min(unsigned int nind, unsigned int dir) const{
    iNode<dim> in = element_array_index(nind);
    in.ind[dir] -= 1;
    return element_serial_index(in);
  }
 
  // get neighbor element on the max side in a given direction
  unsigned int neighbor_element_max(unsigned int nind, unsigned int dir) const{
    iNode<dim> in = element_array_index(nind);
    in.ind[dir] += 1;
    return element_serial_index(in);
  }

  // overload the nearest node operator
  unsigned int nearest_node(const Node<dim> & nd) const{
    iNode<dim>  in;
    for (auto i=0; i<dim; i++){
      in.ind[i] = (unsigned int)(double(m_numnodes.ind[i]-1)*(nd.x[i]-this->m_minpt.x[i])/(this->m_maxpt.x[i]-this->m_minpt.x[i]));
    }
    return node_serial_index(in);
  }


protected:

  iNode<dim>            m_numnodes;
  Node<dim>             m_dx;

};


}
#endif
