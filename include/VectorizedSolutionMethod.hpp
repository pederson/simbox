/** @file VectorizedSolutionMethod.hpp
 *  @brief File with VectorizedSolutionMethod class
 *
 *  This contains the VectorizedSolutionMethod class descriptor
 *
 *  @author D. Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _VectorizedSolutionMethod_H
#define _VectorizedSolutionMethod_H

#include <type_traits>
#include <algorithm>
#include <iostream>

 namespace simbox{

/** @class VectorizedSolutionMethod
 *  @brief VectorizedSolutionMethod class to extend vector-type iteration
 *		   to a functor of the contained object iterator
 *
 *  
 *
 */
template <typename ContainerType,
		  typename StaticMethodFunctor>
class VectorizedSolutionMethod{
private:
	friend class vector_solution_iterator;
	ContainerType * 					mCont;
	std::vector<StaticMethodFunctor> 	mFuncts;

	template <bool is_const>
	class vector_solution_iterator{
	private:
		typedef typename std::conditional<is_const, 
						const VectorizedSolutionMethod, 
						VectorizedSolutionMethod>::type 	container_type;
		typedef typename std::conditional<is_const, 
				typename ContainerType::const_iterator, 
				typename ContainerType::iterator>::type 	iterator_type;

		container_type * mVSM;
		iterator_type 	mIt;
		unsigned int 	mCtr_functs;
		unsigned int 	mCtr_objs;
	public:
		typedef vector_solution_iterator					self_type;
		typedef typename iterator_type::difference_type		difference_type;
		typedef decltype(std::declval<StaticMethodFunctor>().operator()(mIt)) 	value_type;
		typedef value_type & 								reference;
		typedef value_type  								pointer;
		typedef typename iterator_type::iterator_category 	iterator_category;

		vector_solution_iterator(container_type * vcm, iterator_type it, unsigned int fctr)
		: mVSM(vcm), mIt(it), mCtr_functs(fctr), mCtr_objs(0) {};

		// copy assignment
		vector_solution_iterator & operator=(const vector_solution_iterator & cit){
			vector_solution_iterator i(cit);
			std::swap(i,*this);
			return *this;
		}

		pointer operator->() const {return &mVSM->mFuncts[mCtr_functs](mIt);};
		reference operator*() const {return mVSM->mFuncts[mCtr_functs](mIt);};

		self_type operator++(){
			std::cout << "before ++" << std::endl;
			mCtr_functs++;
			std::cout << "++" << std::endl;
			unsigned int mod = mCtr_functs%mVSM->mFuncts.size();
			mCtr_objs += mod;
			mCtr_functs -= mod*mVSM->mFuncts.size();
			mIt += mod;
			std::cout << "am here" << std::endl;
			return *this;
		};
		self_type operator++(int blah) {
			// std::cout << "before ++" << std::endl;
			mCtr_functs++;
			// std::cout << "++" << std::endl;
			unsigned int mod = mCtr_functs/mVSM->mFuncts.size();
			// std::cout << "mod: " << mod << std::endl;
			mCtr_objs += mod;
			// std::cout << "functs size: " << mVSM->mFuncts.size();
			// std::cout << "ctr_objs += mod" << std::endl;
			mCtr_functs -= mod*mVSM->mFuncts.size();
			// std::cout << "ctr_functs -= mod*n" << std::endl;
			mIt += mod;
			// std::cout << "mit += mod" << std::endl;
			return *this;
		};

		bool operator!=(const self_type & leaf) const {return mIt != leaf.mIt || mCtr_functs != leaf.mCtr_functs;};
		bool operator==(const self_type & leaf) const {return mIt == leaf.mIt && mCtr_functs == leaf.mCtr_functs;};
	};
public:
	typedef vector_solution_iterator<true> 		const_iterator;
	typedef vector_solution_iterator<false> 	iterator;

	VectorizedSolutionMethod(ContainerType & c, std::vector<StaticMethodFunctor> v) : mCont(&c), mFuncts(v){};

	decltype(mCont->size()) size() const {return mFuncts.size()*mCont->size();};

	iterator begin() {return iterator(this, mCont->begin(), 0);};
	iterator end()	 {return iterator(this, mCont->end(), 0);};

	const_iterator cbegin() const {return const_iterator(this, mCont->cbegin(), 0);};
	const_iterator cend() const	 {return const_iterator(this, mCont->cend(), mFuncts.size());};

	// VectorizedSolution & vectorize() {return *this;};
};

} // end namespace simbox
#endif