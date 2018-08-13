/** @file DETAIL.hpp
 *  @brief file with DETAIL base
 *
 *  This contains the DETAIL base
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _DETAIL_H
#define _DETAIL_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

 namespace simbox{





 	template <class IteratorType, 
			  class UnaryFunction,
			  class InterfaceFunctor>
	void for_each_interface(IteratorType beg, IteratorType end, UnaryFunction u, InterfaceFunctor interf){
		std::for_each(beg, end, [&u, &interf](typename IteratorType::reference itr){return u(interf(itr));});
	}


	// here the InterfacePolicy is explicitly provided as a template argument
	// e.g.:     for_each<MyPolicy>(a.begin(), a.end(), Functor())
	template <class InterfacePolicy,
			  class IteratorType, 
			  class UnaryFunction>
	void for_each(IteratorType beg, IteratorType end, UnaryFunction u){
		std::for_each(beg, end, [&u](typename IteratorType::reference itr){return u(InterfacePolicy::get(itr));});
	}


 namespace Detail{
	// compile-time for_each on a tuple
	template <typename TupleType, typename FunctionType>
	void for_each(TupleType&&, FunctionType
	            , std::integral_constant<std::size_t, std::tuple_size<typename std::remove_reference<TupleType>::type >::value>) {}

	template <std::size_t I, typename TupleType, typename FunctionType
	       , typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type >
	void for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
	{
	    f(std::get<I>(t));
	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
	}

	template <typename TupleType, typename FunctionType>
	void for_each(TupleType&& t, FunctionType f)
	{
	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
	}





	// compile-time for_each on two tuples... used for comparisons
	template <typename TupleType1, typename TupleType2, typename FunctionType>
	void for_each(TupleType1&&, TupleType2&&, FunctionType
	            , std::integral_constant<std::size_t, std::tuple_size<typename std::remove_reference<TupleType1>::type >::value>) {}

	template <std::size_t I, typename TupleType1, typename TupleType2, typename FunctionType
	       , typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType1>::type>::value>::type >
	void for_each(TupleType1&& t1, TupleType2&& t2, FunctionType f, std::integral_constant<size_t, I>)
	{
	    f(std::get<I>(t1), std::get<I>(t2));
	    for_each(std::forward<TupleType1>(t1), std::forward<TupleType2>(t2), f, std::integral_constant<size_t, I + 1>());
	}

	template <typename TupleType1, typename TupleType2, typename FunctionType>
	void for_each(TupleType1&& t1, TupleType2&& t2, FunctionType f)
	{
	    for_each(std::forward<TupleType1>(t1), std::forward<TupleType2>(t2), f, std::integral_constant<size_t, 0>());
	}





	template <std::size_t... Ns >
	struct indices
	{
	    typedef indices< Ns..., sizeof...( Ns ) > next;
	};

	template <std::size_t N >
	struct make_indices
	{
	    typedef typename make_indices< N - 1 >::type::next type;
	};

	template <>
	struct make_indices< 0 >
	{
	    typedef indices<> type;
	};






	// replace all types in the tuple
	template< typename Tuple,
	          typename Indices = typename make_indices< std::tuple_size< Tuple >::value >::type >
	struct element_replace_all_ref;

	template< typename... Ts, std::size_t... Ns >
	struct element_replace_all_ref< std::tuple< Ts... >, indices< Ns... > >
	{
	    typedef std::tuple< typename Ts::reference... > type;
	};




	// make a tuple of references from a tuple
	template <typename... Args, std::size_t... Is>
	typename Detail::element_replace_all_ref<std::tuple<Args...>>::type 
	make_tuple_ref(std::tuple<Args...>& t, std::index_sequence<Is...>)
	{
	    return std::tie(*std::get<Is>(t)...);
	}


	template <typename... Args>
	typename Detail::element_replace_all_ref<std::tuple<Args...>>::type make_tuple_ref(std::tuple<Args...>& t)
	{
	    return make_tuple_ref(t, std::make_index_sequence<sizeof...(Args)>());
	}
} // end namespace Detail

} // end namespace simbox
#endif