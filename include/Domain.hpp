/** @file Domain.hpp
 *  @brief File with Domain class
 *
 *  This contains the Domain class descriptor
 *
 *  @author D. Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _DOMAIN_H
#define _DOMAIN_H

 namespace simbox{

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

	/** @brief number of nodes in a given processor
	 *
	 *  The number of nodes that a given
	 *	processor is responsible for
	 *
	 *	@param proc processor rank
	 *  @return number of nodes
	 */
	virtual unsigned int nodecount(int proc) const = 0;

	/** @brief number of elements in a given processor
	 *
	 *  The number of elements that a given
	 *	processor is responsible for
	 *
	 *	@param proc processor rank
	 *  @return number of elements
	 */
	virtual unsigned int elementcount(int proc) const = 0;
	
	/** @brief total number of nodes in domain
	 *
	 *  @return number of nodes
	 */
	virtual unsigned int nodecount() const = 0;

	/** @brief total number of elements in domain
	 *
	 *  @return number of elements
	 */
	virtual unsigned int elementcount() const = 0;

	/** @brief total number of procs in domain
	 *
	 *  @return number of elements
	 */
	virtual unsigned int numprocs() const = 0;

};

}
#endif