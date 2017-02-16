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


enum class Direction : unsigned int{X=0,Y,Z};





/** @class iNode
 *  @brief a single index node
 *
 *  an node of indices in <dim> dimensions 
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
