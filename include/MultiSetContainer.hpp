#ifndef _MULTISETCONTAINER_H
#define _MULTISETCONTAINER_H


#include <unordered_map>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <functional>

namespace simbox{


template <typename container_type>
struct container_value_type{
	typedef typename std::remove_reference<decltype(*std::declval<container_type>().begin())>::type type;
};

template <typename value_type>
struct key_type{
	typedef unsigned int type;
};

template <typename key, typename value>
struct key_type<std::pair<const key, value>>{
	typedef key type;
};



// container that contains the sets with key-reference pairs
// the iterator for this should be dereferenced as the same type
// of iterator according to the key type
template <typename KeyT, typename ValueT>
struct set_container : public std::unordered_map<KeyT, ValueT *>{
private:
	typedef std::unordered_map<KeyT, ValueT *> 		base_type;
	typedef set_container 							set_self_type;

	template <bool is_const>
	struct set_container_iterator{
	private:
		typedef typename std::conditional<is_const, typename base_type::const_iterator, typename base_type::iterator>::type base_iterator;
		base_iterator 		mIt;
		typename std::conditional<is_const, typename std::add_const<set_self_type>::type, set_self_type>::type * mCont;
	public:
		typedef ValueT								orig_value_type;
		typedef set_container_iterator				self_type;
		typedef std::ptrdiff_t 						difference_type;
	    typedef typename std::conditional<is_const, 
	    			typename std::add_const<orig_value_type>::type, 
	    			orig_value_type>::type 						
	    								  			value_type;
	    typedef value_type &			  			reference;
	    typedef value_type *						pointer;
	    typedef std::random_access_iterator_tag		iterator_category;

		// construction
		set_container_iterator(set_container * m, base_iterator it)
		: mCont(m), mIt(it){};

		// copy assignment
		set_container_iterator & operator=(const set_container_iterator & cit){
			set_container_iterator i(cit);
			std::swap(i,*this);
			return *this;
		}

		pointer operator->() const {return mIt->operator*();};
		reference operator*() const {return *mIt->operator*();};

		// increment operators
		self_type operator++(){
			mIt++;
			return *this;
		}
		self_type operator++(int blah){
			mIt++;
			return *this;
		}

		// decrement operators
		self_type operator--(){
			mIt--;
			return *this;
		}
		self_type operator--(int blah){
			mIt--;
			return *this;
		}

		// equivalence operators
		bool operator!=(const self_type & leaf) const {return mIt != leaf.mIt;};
		bool operator==(const self_type & leaf) const {return mIt == leaf.mIt;};

	};

public:

	typedef set_container_iterator<true> 	const_iterator;
	typedef set_container_iterator<false> 	iterator;

	iterator begin() {return iterator(this, base_type::begin());};
	iterator end()	 {return iterator(this, base_type::end());};

	const_iterator cbegin() const {return const_iterator(this, base_type::cbegin());};
	const_iterator cend() const	 {return const_iterator(this, base_type::cend());};

};




// MultiSetContainer is a mapped container type that organizes its elements into sets.
//* 	Each key-value pair could possibly be in multiple sets (i.e. the intersection
//* 	of two sets can be nonzero). 
//*
//* 	Ordinary iteration over all elements is available with the ::iterator type and
//* 	can be accessed using the begin() and end() functions. 
//* 
//* 	The unique aspect of this container is its ability to iterate over sets. 
//* 	Iteration over sets is available with the ::set_iterator type, and can be 
//* 	conveniently accessed using the set_begin(SetType s) and set_end(SetType s) functions
//*
//* 	Furthermore, iteration through the available SetType values can be done
//* 	via the ::set_enumerator type
//*
//***********************************************************/
template <typename SetType, typename Derived>
struct MultiSetContainer{
private:
	typedef SetType													set_type;
	typedef Derived 												container_type;
	typedef typename container_value_type<container_type>::type 	value_type;
	typedef typename key_type<value_type>::type 					key_type;
	typedef set_container<key_type, value_type>						set_container_type;

	typedef std::unordered_multimap<key_type, set_type> 			multimap_type;		// this manages the subdomains for every key
	typedef std::map<set_type, set_container_type>					set_map_type;		// this maps from a set_type to a set_container

	// member data
	multimap_type     				mMultiMap;
	set_map_type 					mSetMap;

	Derived & derived() {return &static_cast<Derived *>(this);};
	const Derived & derived() const {return &static_cast<const Derived *>(this);};

public:
	using iterator = typename Derived::iterator;

	set_container_type & set(set_type s){return mSetMap[s];};

	// add an existing element to a set "s"
	void add_to_set(iterator & it, set_type s){
		mMultiMap.emplace(it->first, s);		// create entry in the multimap
		mSetMap[s].insert(std::make_pair(it->first, &(*it)));	// create entry in the set container
	}

	// // remove an existing element from set "s"
	// void remove_from_set(iterator & it, SetT s){
	// 	// erase from the multimap
	// 	auto mit = mMultiMap.find(it->first);
	// 	while (mit->second != s && mit->first == it->first){
	// 		mit++;
	// 	}
	// 	if (mit->second == s && mit->first == it->first){
	// 		mMultiMap.erase(mit);
	// 		// erase from set container
	// 		auto sit = mSetMap[s].find(it->first);
	// 		mSetMap[s].erase(sit);
	// 		// remove the set if it is now empty
	// 		if (mSetMap[s].empty()){
	// 			auto rit = mSetMap.find(s);
	// 			mSetMap.erase(rit);
	// 		}
	// 	}
	// }


	// set intersection
	// set difference
	// set union

};



/*

template <class NContainer, class SetT, class UnaryFunction>
void for_each_set(NContainer & c, std::vector<SetT> & sets, UnaryFunction f){
	for (auto it = sets.begin(); it!=sets.end(); it++){
		std::for_each(c.set_begin(*it), c.set_end(*it), f);
	}
}


template <class NContainer, class SetT, class UnaryFunction>
void for_each_set(NContainer & c, std::vector<SetT> & sets, std::map<SetT, UnaryFunction> & fmap){
	for (auto it = sets.begin(); it!=sets.end(); it++){
		std::for_each(c.set_begin(*it), c.set_end(*it), fmap.at(*it));
	}
}

*/

}// end namespace simbox

#endif