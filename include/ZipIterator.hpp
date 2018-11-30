#ifndef _ZIPITERATOR_H
#define _ZIPITERATOR_H


#include <tuple>
#include <type_traits>

#include "Detail.hpp"

namespace simbox{




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
	template <std::size_t I>
	typename std::tuple_element<I, IteratorTuple>::type & 
	get() {return *std::get<I>(mtup);};

	// accessing a given iterator in the zipped container
	template <std::size_t I>
	const typename std::tuple_element<I, IteratorTuple>::type & 
	get() const {return *std::get<I>(mtup);};

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

// convenience function to create a zipped iterator
template <typename... Iterator>
ZipIterator<std::tuple<Iterator...>> make_zip_iterator(Iterator... it){
	return ZipIterator<std::tuple<Iterator...>>(std::make_tuple(it...));
}


} // end namespace simbox

#endif