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

#include <omp.h>

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
	void for_each(IteratorType beg, IteratorType end, Functor u, Args && ... a){
		std::for_each(beg, end, [&u, &a...](auto & itr){return u.operator()(itr, std::forward<Args>(a)...);});
	}


	// // parallelized version for random-access iterator types
	// // for_each iteration over a general functor. The first argument to the functor
	// // must be the dereferenced iterator type. Following arguments (if any) are passed
	// // successively to the functor.
	// //
	// // e.g.:	for_each(a.begin(), a.end(), Functor(), arg1, arg2);
	// // 			for_each(a.begin(), a.end(), Functor());
	// template <class IteratorType, 
	// 		  class Functor,
	// 		  typename... Args>
	// void for_each_parallel(IteratorType beg, IteratorType end, Functor u, Args... a){
	// 	static_assert(std::is_same<typename IteratorType::iterator_category, std::random_access_iterator_tag>::value, "Must be a random access iterator to parallelize!");
	// 	std::size_t size = end - beg;

	// 	#pragma omp parallel default(shared)
	// 	{

	// 		#pragma omp for
	// 		for (int i=0; i<size; i++){
	// 			u.operator()(*(beg+i), a...);
	// 		}
	// 	}
	// }



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
	void for_each(IteratorType beg, IteratorType end, Functor u, Args && ... a){
		std::for_each(beg, end, [&u, &a...](auto & itr){return u.operator()(InterfacePolicy::get(itr), std::forward<Args>(a)...);});
	}


	// parallelized version for random-access iterator types
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
	void for_each_parallel(IteratorType beg, IteratorType end, Functor u, Args... a){
		static_assert(std::is_same<typename IteratorType::iterator_category, std::random_access_iterator_tag>::value, "Must be a random access iterator to parallelize!");
		std::size_t size = end - beg;
		if (size == 0) return;



		// omp_lock_t writelock;

		// omp_init_lock(&writelock);





		// // std::cout << "size: " << size << std::endl;
		// #pragma omp parallel default(shared)
		// {
		// 	omp_set_lock(&writelock);
		// 	std::cout << "thread: " << omp_get_thread_num() << " begin: " << size*omp_get_thread_num()/omp_get_num_threads() << std::endl;
  //   		omp_unset_lock(&writelock);

		// 	for (IteratorType it=beg+size*omp_get_thread_num()/omp_get_num_threads(); it!=end; it++){
		// 		u.operator()(InterfacePolicy::get(*(it)), a...);
		// 	}

		// }

		// omp_destroy_lock(&writelock);

		// #pragma omp parallel default(shared)
		// {
			#pragma omp parallel for schedule(static) shared(beg,end,u)
			for (int i=0; i<size; i++){
				u.operator()(InterfacePolicy::get(*(beg+i)), a...);
			}
		// }
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