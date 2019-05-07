#ifndef _MULTISETCONTAINER_H
#define _MULTISETCONTAINER_H


#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <functional>

namespace simbox{


template <typename container_type>
struct container_value_type{
	typedef typename std::remove_reference<decltype(std::declval<container_type>().begin().operator*())>::type type;
};

template <typename Iterator>
struct is_pair : public std::false_type {};

template <typename f, typename s>
struct is_pair<std::pair<f,s>> : public std::true_type {};



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

		pointer operator->() const {return mIt->second;};
		reference operator*() const {return *mIt->second;};

		KeyT key() const {return mIt->first;};

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

		// // random access iterator operators
		// template <typename T = base_iterator>
		// typename std::enable_if<std::is_same<typename std::iterator_traits<T>::iterator_category, std::random_access_iterator_tag>::value, difference_type>::type 
		// operator-(const self_type & it) const {return mIt-it.mIt;};
	};

public:

	typedef set_container_iterator<true> 	const_iterator;
	typedef set_container_iterator<false> 	iterator;

	iterator begin() {return iterator(this, base_type::begin());};
	iterator end()	 {return iterator(this, base_type::end());};

	const_iterator cbegin() const {return const_iterator(this, base_type::cbegin());};
	const_iterator cend() const	 {return const_iterator(this, base_type::cend());};


	iterator find(KeyT k) {return iterator(this, base_type::find(k));};

};
// */



/*
// container that contains the sets with key-reference pairs
// the iterator for this should be dereferenced as the same type
// of iterator according to the key type
template <typename KeyT, typename ValueT>
struct set_container : public std::vector<ValueT *>{
private:
	// static_assert(!is_pair<ValueT>::value, "This is a pair" );
	typedef std::vector<ValueT *> 				base_type;
	typedef std::unordered_map<KeyT, std::size_t>		key_storage_type;
	typedef set_container 						set_self_type;

	key_storage_type 							mKeyStore;

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

		pointer operator->() {return *mIt;};  // returns ValueT *
		reference operator*() {return *(*mIt);}; // returns ValueT &

		// increment operators
		self_type operator++(){
			mIt++;
			return *this;
		}
		self_type operator++(int blah){
			mIt++;
			return *this;
		}
		self_type operator+=(std::size_t n){
			mIt += n;
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
		self_type operator-=(std::size_t n){
			mIt -= n;
			return *this;
		}
		

		// equivalence operators
		bool operator!=(const self_type & leaf) const {return mIt != leaf.mIt;};
		bool operator==(const self_type & leaf) const {return mIt == leaf.mIt;};

		// random access operators
		difference_type operator-(const self_type & it) const {
			return mIt - it.mIt;
		}
		self_type operator-(std::size_t n) const {
			return self_type(mCont, mIt - n);
		}
		self_type operator+(std::size_t n) const {
			return self_type(mCont, mIt + n);
		}
		
	};

public:

	typedef set_container_iterator<true> 	const_iterator;
	typedef set_container_iterator<false> 	iterator;

	iterator begin() {return iterator(this, base_type::begin());};
	iterator end()	 {return iterator(this, base_type::end());};

	const_iterator cbegin() const {return const_iterator(this, base_type::cbegin());};
	const_iterator cend() const	 {return const_iterator(this, base_type::cend());};


	iterator find(KeyT k) {
		auto kit = mKeyStore.find(k);
		return (kit == mKeyStore.end() ? iterator(this, base_type::end()) : iterator(this, base_type::begin()+kit->second));
	};

	iterator insert(KeyT k, ValueT * v){
		base_type::push_back(v);
		mKeyStore[k] = base_type::size()-1;
	}


};
// */




/*
// container that contains the sets with key-reference pairs
// the iterator for this should be dereferenced as the same type
// of iterator according to the key type
template <typename KeyT, typename ValueT>
struct set_container : public std::vector<ValueT *>{
private:
	// static_assert(!is_pair<ValueT>::value, "This is a pair" );
	typedef std::vector<ValueT *> 				base_type;
	typedef std::unordered_map<KeyT, std::size_t>		key_storage_type;
	typedef set_container 						set_self_type;

	// typedef  base_iterator;
	typedef typename base_type::iterator base_iterator;

	key_storage_type 							mKeyStore;


	template <bool is_const>
	struct set_container_iterator : public base_iterator {
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

	    using base_iterator::base_iterator;
		// // construction
		// set_container_iterator(set_container * m, base_iterator it)
		// : mCont(m), mIt(it){};

		// // copy assignment
		// set_container_iterator & operator=(const set_container_iterator & cit){
		// 	set_container_iterator i(cit);
		// 	std::swap(i,*this);
		// 	return *this;
		// }

		pointer operator->() {return *(*base_iterator::operator*());};  // returns ValueT *
		reference operator*() {return *(*(*(base_iterator::operator*())));}; // returns ValueT &

		// // increment operators
		// self_type operator++(){
		// 	mIt++;
		// 	return *this;
		// }
		// self_type operator++(int blah){
		// 	mIt++;
		// 	return *this;
		// }
		// self_type operator+=(std::size_t n){
		// 	mIt += n;
		// 	return *this;
		// }

		// // decrement operators
		// self_type operator--(){
		// 	mIt--;
		// 	return *this;
		// }
		// self_type operator--(int blah){
		// 	mIt--;
		// 	return *this;
		// }
		// self_type operator-=(std::size_t n){
		// 	mIt -= n;
		// 	return *this;
		// }
		

		// // equivalence operators
		// bool operator!=(const self_type & leaf) const {return mIt != leaf.mIt;};
		// bool operator==(const self_type & leaf) const {return mIt == leaf.mIt;};

		// // random access operators
		// difference_type operator-(const self_type & it) const {
		// 	return mIt - it.mIt;
		// }
		// self_type operator-(std::size_t n) const {
		// 	return self_type(mCont, mIt - n);
		// }
		// self_type operator+(std::size_t n) const {
		// 	return self_type(mCont, mIt + n);
		// }
		
	};

public:

	typedef set_container_iterator<true> 	const_iterator;
	typedef set_container_iterator<false> 	iterator;

	using base_type::begin;
	using base_type::end;
	using base_type::cbegin;
	using base_type::cend;
	// iterator begin() {return iterator(this, base_type::begin());};
	// iterator end()	 {return iterator(this, base_type::end());};

	// const_iterator cbegin() const {return const_iterator(this, base_type::cbegin());};
	// const_iterator cend() const	 {return const_iterator(this, base_type::cend());};


	// iterator find(KeyT k) {
	// 	auto kit = mKeyStore.find(k);
	// 	return (kit == mKeyStore.end() ? iterator(this, base_type::end()) : iterator(this, base_type::begin()+kit->second));
	// };

	iterator find(KeyT k) {
		auto kit = mKeyStore.find(k);
		return (kit == mKeyStore.end() ? iterator(base_type::end()) : iterator(base_type::begin() + kit->second));
	};

	iterator insert(KeyT k, ValueT * v){
		base_type::push_back(v);
		mKeyStore[k] = base_type::size()-1;
	}


};
// */












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
struct MultiSetContainer : public Derived{
public:

	typedef typename Derived::iterator iterator;

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

	Derived & derived() {return *static_cast<Derived *>(this);};
	const Derived & derived() const {return *static_cast<const Derived *>(this);};

	template <typename T = value_type>
	typename std::enable_if<is_pair<T>::value, key_type>::type 
	get_key(const iterator & it) const {return it->first;};

	template <typename T = value_type>
	typename std::enable_if<!is_pair<T>::value, key_type>::type 
	get_key(const iterator & it) const {return it - derived().begin();};
public:

	set_container_type & set(set_type s){return mSetMap[s];};

	// add an existing element to a set "s"
	void  add_to_set(const iterator & it, set_type s){
		// std::cout << "adding key: " << get_key(it) << " to set " << s << std::endl;
		mMultiMap.emplace(get_key(it), s);		// create entry in the multimap
		mSetMap[s].insert(std::make_pair(get_key(it), &(*it)));	// create entry in the set container
		// mSetMap[s].insert(get_key(it), &(*it));	// create entry in the set container
	}

	// remove an existing element from set "s"
	void remove_from_set(const iterator & it, set_type s){
		// erase from the multimap
		key_type my_key = get_key(it);
		auto mit = mMultiMap.find(my_key);
		while (mit->second != s && mit->first == my_key){
			mit++;
		}
		if (mit->second == s && mit->first == my_key){
			mMultiMap.erase(mit);
			// erase from set container
			auto sit = mSetMap[s].find(my_key);
			mSetMap[s].erase(sit);
			// remove the set if it is now empty
			if (mSetMap[s].empty()){
				auto rit = mSetMap.find(s);
				mSetMap.erase(rit);
			}
		}
	}

	// set enumerator
	std::vector<set_type> enumerate_sets() const {
		std::vector<set_type> out;
		for (auto it = mSetMap.begin(); it != mSetMap.end(); it++){
			out.push_back(it->first);
		}
		return out;
	}



	// the following logical operations return a vector of keys (either a key in the
	// case of a map container, or an index (integer) in the case of a vector container)
	// set intersection
	std::vector<key_type> set_intersection(set_type s1, set_type s2){
		std::vector<key_type> k1, k2;

		k1.reserve(set(s1).size());
		k2.reserve(set(s2).size());

		for (auto it = set(s1).begin(); it != set(s1).end(); it++) k1.push_back(it->first);
		for (auto it = set(s2).begin(); it != set(s2).end(); it++) k2.push_back(it->first);

		std::sort(k1.begin(), k1.end());
		std::sort(k2.begin(), k2.end());

		std::vector<key_type> out;

		std::set_intersection(k1.begin(), k1.end(), k2.begin(), k2.end(), std::back_inserter(out));

		return out;
	}

	// set difference
	std::vector<key_type> set_difference(set_type s1, set_type s2){
		std::vector<key_type> k1, k2;

		k1.reserve(set(s1).size());
		k2.reserve(set(s2).size());

		for (auto it = set(s1).begin(); it != set(s1).end(); it++) k1.push_back(it->first);
		for (auto it = set(s2).begin(); it != set(s2).end(); it++) k2.push_back(it->first);

		std::sort(k1.begin(), k1.end());
		std::sort(k2.begin(), k2.end());

		std::vector<key_type> out;

		std::set_difference(k1.begin(), k1.end(), k2.begin(), k2.end(), std::back_inserter(out));

		return out;
	}


	// set union
	std::vector<key_type> set_union(set_type s1, set_type s2){
		std::vector<key_type> k1, k2;

		k1.reserve(set(s1).size());
		k2.reserve(set(s2).size());

		for (auto it = set(s1).begin(); it != set(s1).end(); it++) k1.push_back(it->first);
		for (auto it = set(s2).begin(); it != set(s2).end(); it++) k2.push_back(it->first);

		std::sort(k1.begin(), k1.end());
		std::sort(k2.begin(), k2.end());

		std::vector<key_type> out;

		std::set_union(k1.begin(), k1.end(), k2.begin(), k2.end(), std::back_inserter(out));

		return out;
	}

	// set xor
	std::vector<key_type> set_xor(set_type s1, set_type s2){
		std::vector<key_type> k1, k2;

		k1.reserve(set(s1).size());
		k2.reserve(set(s2).size());

		for (auto it = set(s1).begin(); it != set(s1).end(); it++) k1.push_back(it->first);
		for (auto it = set(s2).begin(); it != set(s2).end(); it++) k2.push_back(it->first);

		std::sort(k1.begin(), k1.end());
		std::sort(k2.begin(), k2.end());

		std::vector<key_type> out;

		std::set_symmetric_difference(k1.begin(), k1.end(), k2.begin(), k2.end(), std::back_inserter(out));

		return out;
	}

};




template <typename key, typename value, typename set>
using set_map = MultiSetContainer<set, std::map<key, value>>;

template <typename key, typename value, typename set>
using set_unordered_map = MultiSetContainer<set, std::unordered_map<key, value>>;

template <typename value, typename set>
using set_vector = MultiSetContainer<set, std::vector<value>>;



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