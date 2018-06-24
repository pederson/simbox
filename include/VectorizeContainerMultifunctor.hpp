/** @file VectorizeContainerMultifunctor.hpp
 *  @brief File with VectorizeContainerMultifunctor class
 *
 *  This contains the VectorizeContainerMultifunctor class descriptor
 *
 *  @author D. Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _VECTORIZECONTAINERMULTIFUNCTOR_H
#define _VECTORIZECONTAINERMULTIFUNCTOR_H

#include <type_traits>
#include <algorithm>
#include <iostream>

#include "Macros.hpp"

 namespace simbox{

/** @class VectorizeContainerMultifunctor
 *  @brief VectorizeContainerMultifunctor class to extend vector-type iteration
 *		   to a functor of the contained object iterator
 *
 *  
 *
 */
template <typename ContainerType,
		  typename StaticMethodFunctor>
class VectorizeContainerMultifunctor{
private:
	friend class vector_solution_iterator;
	ContainerType * 					mCont;
	std::vector<StaticMethodFunctor> 	mFuncts;

	template <bool is_const>
	class vector_solution_iterator{
	private:
		typedef typename std::conditional<is_const, 
						const VectorizeContainerMultifunctor, 
						VectorizeContainerMultifunctor>::type 	container_type;
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
			mCtr_functs++;
			unsigned int mod = mCtr_functs/mVSM->mFuncts.size();
			mCtr_objs += mod;
			mCtr_functs -= mod*mVSM->mFuncts.size();
			mIt += mod;
			return *this;
		};
		self_type operator++(int blah) {
			mCtr_functs++;
			unsigned int mod = mCtr_functs/mVSM->mFuncts.size();
			mCtr_objs += mod;
			mCtr_functs -= mod*mVSM->mFuncts.size();
			mIt += mod;
			return *this;
		};

		bool operator!=(const self_type & leaf) const {return mIt != leaf.mIt || mCtr_functs != leaf.mCtr_functs;};
		bool operator==(const self_type & leaf) const {return mIt == leaf.mIt && mCtr_functs == leaf.mCtr_functs;};
	};
public:
	typedef vector_solution_iterator<true> 		const_iterator;
	typedef vector_solution_iterator<false> 	iterator;

	VectorizeContainerMultifunctor(ContainerType & c, std::vector<StaticMethodFunctor> v) : mCont(&c), mFuncts(v){};

	decltype(mCont->size()) size() const {return mFuncts.size()*mCont->size();};

	iterator begin() {return iterator(this, mCont->begin(), 0);};
	iterator end()	 {return iterator(this, mCont->end(), 0);};

	const_iterator cbegin() const {return const_iterator(this, mCont->cbegin(), 0);};
	const_iterator cend() const	 {return const_iterator(this, mCont->cend(), mFuncts.size());};
};








#define SIMBOX_VECTORIZE_MULTIFUNCTOR(ResultName) CRTP_Vectorize_Multifunctor_##ResultName
#define SIMBOX_VECTORIZE_MULTIFUNCTOR_DEF(ResultName, FunctorType, ...)					\
	template <typename ContainerT, typename Functor>									\
	using VSM = simbox::VectorizeContainerMultifunctor<ContainerT, Functor>;					\
																						\
	namespace detail{																	\
		namespace vectorize_multifunctor{												\
			namespace ResultName {														\
				SIMBOX_FOR_EACH_SEP(SIMBOX_FUNCTOR_PREPARE, __VA_ARGS__);				\
				std::vector<functor_type> functs = 										\
				{SIMBOX_FOR_EACH_SEQ(SIMBOX_INSTANTIATE_FUNCTOR_FOR, __VA_ARGS__)};		\
			}																			\
		} 																				\
	}																					\
																						\
	template <typename Derived> 														\
	struct SIMBOX_VECTORIZE_MULTIFUNCTOR(ResultName){ 									\
	private: 																			\
		Derived & derived() {return *static_cast<Derived *>(this);};					\
		const Derived & derived() const {return *static_cast<const Derived *>(this);};	\
	public: 																			\
		VSM<Derived, FunctorType> 														\
		ResultName() 																	\
		{return VSM<Derived, FunctorType>(derived(), 									\
					detail::vectorize_multifunctor::ResultName::functs);};				\
	};






} // end namespace simbox
#endif