/** @file VectorizeContainerFunctor.hpp
 *  @brief File with VectorizeContainerFunctor class
 *
 *  This contains the VectorizeContainerFunctor class descriptor
 *
 *  @author D. Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _VECTORIZECONTAINERFUNCTOR_H
#define _VECTORIZECONTAINERFUNCTOR_H

#include <type_traits>
#include <algorithm>

#include "Macros.hpp"

 namespace simbox{

/** @class VectorizeContainerFunctor
 *  @brief VectorizeContainerFunctor class to extend vector-type iteration
 *		   to a functor of the contained object iterator
 *
 *  
 *
 */
template <typename ContainerType,
		  typename StaticMethodFunctor>
class VectorizeContainerFunctor{
private:
	ContainerType * mCont;

	template <bool is_const>
	class vcm_iterator{
	private:
		typedef typename std::conditional<is_const, 
						const VectorizeContainerFunctor, 
						VectorizeContainerFunctor>::type 	container_type;
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

	VectorizeContainerFunctor(ContainerType & c) : mCont(&c) {};

	decltype(mCont->size()) size() const {return mCont->size();};

	iterator begin() {return iterator(this, mCont->begin());};
	iterator end()	 {return iterator(this, mCont->end());};

	const_iterator cbegin() const {return const_iterator(this, mCont->cbegin());};
	const_iterator cend() const	 {return const_iterator(this, mCont->cend());};


};






#define SIMBOX_VECTORIZE_FUNCTOR(FunctionName) CRTP_Vectorize_Functor_##FunctionName
#define SIMBOX_VECTORIZE_FUNCTOR_DEF(FunctionName)		\
	template <typename ContainerT, typename Functor>	\
	using VCM = simbox::VectorizeContainerFunctor<ContainerT, Functor>;	\
														\
	namespace detail{									\
		namespace vectorize_functor{					\
			namespace FunctionName {					\
				SIMBOX_FUNCTOR_PREPARE(FunctionName);	\
			}											\
		} 												\
	}													\
														\
	template <typename Derived> 						\
	struct SIMBOX_VECTORIZE_FUNCTOR(FunctionName){ 				\
	private: 											\
		Derived & derived() {return *static_cast<Derived *>(this);};	\
		const Derived & derived() const {return *static_cast<const Derived *>(this);};	\
	public: 																			\
		VCM<Derived, detail::vectorize_functor::FunctionName::SIMBOX_FUNCTOR_FOR(FunctionName)> 	\
		FunctionName() 																	\
		{return VCM<Derived, detail::vectorize_functor::FunctionName::SIMBOX_FUNCTOR_FOR(FunctionName)>(derived());};	\
	};








} // end namespace simbox
#endif