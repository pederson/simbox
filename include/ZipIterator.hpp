#ifndef _ZIPITERATOR_H
#define _ZIPITERATOR_H


#include <tuple>
#include <type_traits>

#include "Detail.hpp"

namespace simbox{



// namespace Detail{
// 	// compile-time for_each on a tuple
// 	template <typename TupleType, typename FunctionType>
// 	void for_each(TupleType&&, FunctionType
// 	            , std::integral_constant<std::size_t, std::tuple_size<typename std::remove_reference<TupleType>::type >::value>) {}

// 	template <std::size_t I, typename TupleType, typename FunctionType
// 	       , typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type >
// 	void for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
// 	{
// 	    f(std::get<I>(t));
// 	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
// 	}

// 	template <typename TupleType, typename FunctionType>
// 	void for_each(TupleType&& t, FunctionType f)
// 	{
// 	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
// 	}





// 	// compile-time for_each on two tuples... used for comparisons
// 	template <typename TupleType1, typename TupleType2, typename FunctionType>
// 	void for_each(TupleType1&&, TupleType2&&, FunctionType
// 	            , std::integral_constant<std::size_t, std::tuple_size<typename std::remove_reference<TupleType1>::type >::value>) {}

// 	template <std::size_t I, typename TupleType1, typename TupleType2, typename FunctionType
// 	       , typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType1>::type>::value>::type >
// 	void for_each(TupleType1&& t1, TupleType2&& t2, FunctionType f, std::integral_constant<size_t, I>)
// 	{
// 	    f(std::get<I>(t1), std::get<I>(t2));
// 	    for_each(std::forward<TupleType1>(t1), std::forward<TupleType2>(t2), f, std::integral_constant<size_t, I + 1>());
// 	}

// 	template <typename TupleType1, typename TupleType2, typename FunctionType>
// 	void for_each(TupleType1&& t1, TupleType2&& t2, FunctionType f)
// 	{
// 	    for_each(std::forward<TupleType1>(t1), std::forward<TupleType2>(t2), f, std::integral_constant<size_t, 0>());
// 	}





// 	template <std::size_t... Ns >
// 	struct indices
// 	{
// 	    typedef indices< Ns..., sizeof...( Ns ) > next;
// 	};

// 	template <std::size_t N >
// 	struct make_indices
// 	{
// 	    typedef typename make_indices< N - 1 >::type::next type;
// 	};

// 	template <>
// 	struct make_indices< 0 >
// 	{
// 	    typedef indices<> type;
// 	};






// 	// replace all types in the tuple
// 	template< typename Tuple,
// 	          typename Indices = typename make_indices< std::tuple_size< Tuple >::value >::type >
// 	struct element_replace_all_ref;

// 	template< typename... Ts, std::size_t... Ns >
// 	struct element_replace_all_ref< std::tuple< Ts... >, indices< Ns... > >
// 	{
// 	    typedef std::tuple< typename Ts::reference... > type;
// 	};




// 	// make a tuple of references from a tuple
// 	template <typename... Args, std::size_t... Is>
// 	typename Detail::element_replace_all_ref<std::tuple<Args...>>::type 
// 	make_tuple_ref(std::tuple<Args...>& t, std::index_sequence<Is...>)
// 	{
// 	    return std::tie(*std::get<Is>(t)...);
// 	}


// 	template <typename... Args>
// 	typename Detail::element_replace_all_ref<std::tuple<Args...>>::type make_tuple_ref(std::tuple<Args...>& t)
// 	{
// 	    return make_tuple_ref(t, std::make_index_sequence<sizeof...(Args)>());
// 	}
// } // end namespace detail










template <typename IteratorTuple>
class ZipIterator{
public:
	typedef ZipIterator 					self_type;
	typedef	typename Detail::element_replace_all_ref<IteratorTuple>::type 	reference;
    typedef	reference											value_type;
    typedef	value_type *										pointer;
    typedef typename std::tuple_element<0, IteratorTuple>::type::difference_type	difference_type;
    typedef typename std::tuple_element<0, IteratorTuple>::type::iterator_category	iterator_category;


    ZipIterator(){};

    ZipIterator(IteratorTuple t)
    : mtup(t) {};


    // dereferencing
	constexpr reference operator*(){
		return Detail::make_tuple_ref(mtup);
	};

	// accessing a given iterator in the zipped container
	template <std::size_t i>
	constexpr typename std::tuple_element<i, IteratorTuple>::type::reference 
	get(){return *std::get<i>(mtup);};


	// preincrement 
	self_type operator++(){
	 	Detail::for_each(mtup, [](auto & c){++c;});
		return *this;
	};

	// postincrement 
	self_type operator++(int blah){
		Detail::for_each(mtup, [](auto & c){c++;});
		return *this;
	};

	// pointer
	pointer operator->() {return &this->operator*();};

	// inequality
	bool operator!=(const self_type & leaf) const {
		return mtup != leaf.mtup;
	};

	// equality
	bool operator==(const self_type & leaf) const {
		return mtup == leaf.mtup;
	};


private:
	IteratorTuple			mtup;
};






// convenience function to create a zipped iterator
template <typename IteratorTuple>
ZipIterator<IteratorTuple> make_zip_iterator(IteratorTuple t){
	return ZipIterator<IteratorTuple>(t);
}


} // end namespace simbox

#endif