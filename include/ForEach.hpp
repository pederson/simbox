/** @file DETAIL.hpp
 *  @brief file with DETAIL base
 *
 *  This contains the DETAIL base
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _FOREACH_H
#define _FOREACH_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

 namespace simbox{

 	// for_each iteration over a general functor. The first argument to the functor
	// must be the dereferenced iterator type. Following arguments (if any) are passed
	// successively to the functor.
	//
	// e.g.:	for_each(a.begin(), a.end(), Functor(), arg1, arg2);
	// 			for_each(a.begin(), a.end(), Functor());
	template <class IteratorType, 
			  class Functor,
			  typename... Args>
	void for_each(IteratorType beg, IteratorType end, Functor u, Args... a){
		std::for_each(beg, end, [&u, &a...](auto & itr){return u.operator()(itr, a...);});
	}

	// for_each iteration over a general functor. The first argument to the functor
	// must be the dereferenced iterator type. Following arguments are passes successively
	// to the functor.
	//
	// e.g.:	for_each<MyPolicy>(a.begin(), a.end(), Functor(), arg1, arg2);
	// 			for_each<MyPolicy>(a.begin(), a.end(), Functor());
	template <class InterfacePolicy,
			  class IteratorType, 
			  class Functor,
			  typename... Args>
	void for_each(IteratorType beg, IteratorType end, Functor u, Args... a){
		std::for_each(beg, end, [&u, &a...](auto & itr){return u.operator()(InterfacePolicy::get(itr), a...);});
	}

	// for_each iteration over a general iterator calling a unary function calling an interface functor
	// on the iterator before each unary function call. The interface functor is provided at runtime
	// by passing a functor argument
 	template <class IteratorType, 
			  class Functor,
			  class InterfaceFunctor,
			  typename... Args>
	void for_each_interface(IteratorType beg, IteratorType end, Functor u, InterfaceFunctor interf, Args... a){
		std::for_each(beg, end, [&u, &interf, &a...](typename IteratorType::reference itr){return u(interf(itr), a...);});
	}


} // end namespace simbox
#endif