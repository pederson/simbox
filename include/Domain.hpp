/** @file Domain.hpp
 *  @brief File with Domain class
 *
 *  This contains the Domain class descriptor
 *
 *  @author Dylan Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _DOMAIN_H
#define _DOMAIN_H

/** @class Domain
 *  @brief base class for simulation domain
 *
 *  Abstract base class for Domain classes
 *	that describe a simulation domain. These
 *	simulation domains may possibly be split
 *	for parallel computation and I/O
 *
 */
class Domain{
public:

	/** @brief offset in node array
	 *
	 *  Theoretical offset to the start of
	 *	the given processors nodes in 
	 *	overall node array. This is useful
	 *	when doing I/O into a single file
	 *
	 *	@param proc processor rank
	 *  @return offset to first node
	 */
	virtual unsigned int array_node_offset(int proc) const = 0;
	
	/** @brief offset in element array
	 *
	 *  Theoretical offset to the start of
	 *	the given processors nodes in 
	 *	overall element array. This is useful
	 *	when doing I/O into a single file
	 *
	 *	@param proc processor rank
	 *  @return offset to first element
	 */
	virtual unsigned int array_element_offset(int proc) const = 0;
	
	/** @brief number of nodes in a given processor
	 *
	 *  The number of nodes that a given
	 *	processor is responsible for
	 *
	 *	@param proc processor rank
	 *  @return number of nodes
	 */
	virtual unsigned int nnodes(int proc) const = 0;

	/** @brief number of elements in a given processor
	 *
	 *  The number of elements that a given
	 *	processor is responsible for
	 *
	 *	@param proc processor rank
	 *  @return number of elements
	 */
	virtual unsigned int nelements(int proc) const = 0;
	
	/** @brief total number of nodes in domain
	 *
	 *	@param proc processor rank
	 *  @return number of nodes
	 */
	virtual unsigned int nnodes() const = 0;

	/** @brief total number of elements in domain
	 *
	 *	@param proc processor rank
	 *  @return number of elements
	 */
	virtual unsigned int nelements() const = 0;

};

#endif