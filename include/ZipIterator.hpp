#ifndef _ZIPITERATOR_H
#define _ZIPITERATOR_H


#include <tuple>
#include <type_traits>



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




	



	// // replace a single type in the tuple
	// template< typename Tuple, std::size_t N, typename T,
	//           typename Indices = typename make_indices< std::tuple_size< Tuple >::value >::type >
	// struct element_replace;

	// template< typename... Ts, std::size_t N, typename T, std::size_t... Ns >
	// struct element_replace< std::tuple< Ts... >, N, T, indices< Ns... > >
	// {
	//     typedef std::tuple< typename std::conditional< Ns == N, T, Ts >::type... > type;
	// };





	// replace all types in the tuple
	template< typename Tuple,
	          typename Indices = typename make_indices< std::tuple_size< Tuple >::value >::type >
	struct element_replace_all_ref;

	template< typename... Ts, std::size_t... Ns >
	struct element_replace_all_ref< std::tuple< Ts... >, indices< Ns... > >
	{
	    typedef std::tuple< typename Ts::reference... > type;
	};
}










template <typename IteratorTuple>
class ZipIterator{
public:
	typedef	typename Detail::element_replace_all<IteratorTuple>::type 	reference;
    typedef	reference											value_type;
    typedef	value_type *										pointer;
    typedef typename std::tuple_element<0, decltype(mtup)>::type::difference_type	difference_type;
    typedef typename std::tuple_element<0, decltype(mtup)>::type::iterator_category	iterator_category;


    ZipIterator(){};

    ZipIterator(IteratorTuple t)
    : mtup(t) {};



    // dereferencing
	reference operator*(){
		reference r;
	 	for (auto i=0; i<std::tuple_size(mtup); i++){
	 		r[i] = *mtup[i];
	 	}
	 	return r;
	};

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
		for (auto i=0; i<std::tuple_size(mtup); i++){
	 		if (mtup[i] == leaf[i]) return false;
	 	}
		return true;
	};

	// equality
	bool operator==(const self_type & leaf) const {
		for (auto i=0; i<std::tuple_size(mtup); i++){
	 		if (mtup[i] != leaf[i]) return false;
	 	}
		return true;
	};


private:
	IteratorTuple			mtup;
};




template <typename IteratorTuple>
ZipIterator<IteratorTuple> make_zip_iterator(IteratorTuple t){
	return ZipIterator<IteratorTuple>(t);
}

#endif