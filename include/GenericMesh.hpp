/** @file GenericMesh.hpp
 *  @brief file with generic GenericMesh class
 *
 *  This contains the base GenericMesh class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _GENERICMESH_H
#define _GENERICMESH_H

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


struct NoNode {};
struct NoEdge {};
struct NoCell {};

template <typename T>
struct is_void : std::false_type {};

template <>
struct is_void<void> : std::true_type {};


template <typename... T>
struct type_helper{};

template <typename T, typename _ = void>
struct is_node_container : std::false_type{};

template<typename T>
  struct is_node_container<
          T,
          std::conditional_t<
              false,
              type_helper<
                  decltype(std::declval<T>().nodes())
                  >,
              void
              >
          > : public std::true_type {};



template <typename T, typename _ = void>
struct is_edge_container : std::false_type{};

template<typename T>
  struct is_edge_container<
          T,
          std::conditional_t<
              false,
              type_helper<
                  decltype(std::declval<T>().edges())
                  >,
              void
              >
          > : public std::true_type {};


template <typename T, typename _ = void>
struct is_cell_container : std::false_type{};

template<typename T>
  struct is_cell_container<
          T,
          std::conditional_t<
              false,
              type_helper<
                  decltype(std::declval<T>().cells())
                  >,
              void
              >
          > : public std::true_type {};


/** @class GenericMesh
 *  @brief a GenericMesh 
 *
 *  At its most fundamental level, this is simply an aggregation of 
 *  containers that hold nodes, edges, and cells
 *
 *  All other storage and responsibilities are delegated to the containers
 *  
 *  NodeContainerPolicy - Container that holds nodes... can be as key-value pairs or whatever you'd like
 *
 *  EdgeContainerPolicy - Container that holds edges... similarly, this is loosely defined
 *                  
 *  CellContainerPolicy - Container that holds cells... also loosely defined
 *
 *
 */
template <typename NodeContainerPolicy, 
          typename EdgeContainerPolicy, 
          typename CellContainerPolicy>
class GenericMesh : public std::conditional<!is_void<NodeContainerPolicy>::value, NodeContainerPolicy, NoNode>::type,
                    public std::conditional<!is_void<EdgeContainerPolicy>::value, EdgeContainerPolicy, NoEdge>::type,
                    public std::conditional<!is_void<CellContainerPolicy>::value, CellContainerPolicy, NoCell>::type
{
private:
  typedef typename std::conditional<!is_void<NodeContainerPolicy>::value, NodeContainerPolicy, NoNode>::type   node_policy;
  typedef typename std::conditional<!is_void<EdgeContainerPolicy>::value, EdgeContainerPolicy, NoEdge>::type   edge_policy;
  typedef typename std::conditional<!is_void<CellContainerPolicy>::value, CellContainerPolicy, NoCell>::type   cell_policy;
  

  static_assert(std::is_same<node_policy, NoNode>::value || 
                is_node_container<node_policy>::value, "Node Policy must be either void or have a nodes() method!");
  static_assert(std::is_same<edge_policy, NoEdge>::value || 
                is_edge_container<edge_policy>::value, "Edge Policy must be either void or have a edges() method!");
  static_assert(std::is_same<cell_policy, NoCell>::value || 
                is_cell_container<cell_policy>::value, "Cell Policy must be either void or have a cells() method!");


public:

};



} // end namespace simbox
#endif
