/** @file VectorizeContainerMethod.hpp
 *  @brief File with VectorizeContainerMethod class
 *
 *  This contains the VectorizeContainerMethod class descriptor
 *
 *  @author D. Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _VECTORIZECONTAINERMETHOD_H
#define _VECTORIZECONTAINERMETHOD_H

#include <type_traits>
#include <algorithm>
 // #include "mpitools.hpp"

 namespace simbox{

/** @class VectorizeContainerMethod
 *  @brief VectorizeContainerMethod class to extend vector-type iteration
 *		   to a functor of the contained object iterator
 *
 *  
 *
 */
template <typename ContainerType,
		  typename StaticMethodFunctor>
class VectorizeContainerMethod{
private:
	ContainerType * mCont;

	template <bool is_const>
	class vcm_iterator{
	private:
		typedef typename std::conditional<is_const, 
						const VectorizeContainerMethod, 
						VectorizeContainerMethod>::type 	container_type;
		typedef typename std::conditional<is_const, 
				typename ContainerType::const_iterator, 
				typename ContainerType::iterator>::type 	iterator_type;

		container_type * mVCM;
		iterator_type 	mIt;
	public:
		typedef vcm_iterator								self_type;
		typedef typename iterator_type::difference_type		difference_type;
		typedef decltype(StaticMethodFunctor::get(mIt)) 	value_type;
		typedef value_type & 								reference;
		typedef value_type  								pointer;
		typedef typename iterator_type::iterator_category 	iterator_category;

		vcm_iterator(container_type * vcm, iterator_type it)
		: mVCM(vcm), mIt(it) {};

		// copy assignment
		vcm_iterator & operator=(const vcm_iterator & cit){
			vcm_iterator i(cit);
			std::swap(i,*this);
			return *this;
		}

		pointer operator->() const {return &StaticMethodFunctor::get(mIt);};
		reference operator*() const {return StaticMethodFunctor::get(mIt);};

		self_type operator++(){mIt++; return *this;};
		self_type operator++(int blah) {mIt++; return *this;};

		bool operator!=(const self_type & leaf) const {return mIt != leaf.mIt;};
		bool operator==(const self_type & leaf) const {return mIt == leaf.mIt;};

	};
public:
	typedef vcm_iterator<true> 		const_iterator;
	typedef vcm_iterator<false> 	iterator;

	VectorizeContainerMethod(ContainerType & c) : mCont(&c) {};

	decltype(mCont->size()) size() const {return mCont->size();};

	iterator begin() {return iterator(this, mCont->begin());};
	iterator end()	 {return iterator(this, mCont->end());};

	const_iterator cbegin() const {return const_iterator(this, mCont->cbegin());};
	const_iterator cend() const	 {return const_iterator(this, mCont->cend());};


};

} // end namespace simbox
#endif