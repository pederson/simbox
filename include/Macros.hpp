/** @file Macros.hpp
 *  @brief File with preprocessor Macros
 *
 *
 *  @author D. Pederson
 *  @bug No known bugs. 
 */
 
#ifndef _MACROS_H
#define _MACROS_H

namespace simbox{


// concatenate
#define SIMBOX_CONCATENATE(arg1, arg2)  arg1##arg2
// create a string from the argument
#define SIMBOX_STRINGIZE(arg) #arg

// a comma macro for convenience
#define SIMBOX_COMMA ,
// a semicolon macro for convenience
#define SIMBOX_SEMICOLON ;


// this macro tool will defer the expansion of a macro
#define SIMBOX_DEFER(...) __VA_ARGS__


// SIMBOX_FOR_EACH takes a macro called "what", and applies it
// to every item passed in the variadic argument list. The argument "op"
// is a macro that is expanded in between each application of "what"
#define SIMBOX_FOR_EACH_1(what, op, x, ...) what(x)
#define SIMBOX_FOR_EACH_2(what, op, x, ...)	what(x) op SIMBOX_FOR_EACH_1(what, SIMBOX_DEFER(op), __VA_ARGS__)
#define SIMBOX_FOR_EACH_3(what, op, x, ...)	what(x) op SIMBOX_FOR_EACH_2(what, SIMBOX_DEFER(op), __VA_ARGS__)
#define SIMBOX_FOR_EACH_4(what, op, x, ...) what(x) op SIMBOX_FOR_EACH_3(what, SIMBOX_DEFER(op), __VA_ARGS__)
#define SIMBOX_FOR_EACH_5(what, op, x, ...)	what(x) op SIMBOX_FOR_EACH_4(what, SIMBOX_DEFER(op), __VA_ARGS__)
#define SIMBOX_FOR_EACH_6(what, op, x, ...)	what(x) op SIMBOX_FOR_EACH_5(what, SIMBOX_DEFER(op), __VA_ARGS__)
#define SIMBOX_FOR_EACH_7(what, op, x, ...)	what(x) op SIMBOX_FOR_EACH_6(what, SIMBOX_DEFER(op), __VA_ARGS__)
#define SIMBOX_FOR_EACH_8(what, op, x, ...)	what(x) op SIMBOX_FOR_EACH_7(what, SIMBOX_DEFER(op), __VA_ARGS__)

#define SIMBOX_FOR_EACH_NARG(...) SIMBOX_FOR_EACH_NARG_(__VA_ARGS__, SIMBOX_FOR_EACH_RSEQ_N())
#define SIMBOX_FOR_EACH_NARG_(...) SIMBOX_FOR_EACH_ARG_N(__VA_ARGS__) 
#define SIMBOX_FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N 
#define SIMBOX_FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define SIMBOX_FOR_EACH_(N, what, op, x, ...) SIMBOX_CONCATENATE(SIMBOX_FOR_EACH_, N)(what, SIMBOX_DEFER(op), x, __VA_ARGS__)
#define SIMBOX_FOR_EACH(what, op, x, ...) SIMBOX_FOR_EACH_(SIMBOX_FOR_EACH_NARG(x, __VA_ARGS__), what, SIMBOX_DEFER(op), x, __VA_ARGS__)


// for each that creates a sequence... just inserts commas between items
#define SIMBOX_FOR_EACH_SEQ(what, x, ...) SIMBOX_FOR_EACH(what, SIMBOX_DEFER(SIMBOX_COMMA), x, __VA_ARGS__)

// for each that creates seperation... just inserts semicolons between items
#define SIMBOX_FOR_EACH_SEP(what, x, ...) SIMBOX_FOR_EACH(what, SIMBOX_DEFER(SIMBOX_SEMICOLON), x, __VA_ARGS__)




// helper dummy type
template <typename... Args>
struct SimboxHasMethodHelper{};

// macro for defining the struct SimboxHasMethod##MethodName
#define SIMBOX_HAS_METHOD_DEF(MethodName) 						\
	template<typename T, typename _ = void> 					\
	struct SimboxHasMethod##MethodName : std::false_type {};	\
																\
	template <typename T>										\
	struct SimboxHasMethod##MethodName <						\
	        T,													\
	        std::conditional_t<									\
	            false,											\
	            simbox::SimboxHasMethodHelper<					\
	                decltype(std::declval<T>().MethodName())	\
	                >,											\
	            void											\
	            >												\
	        > : public std::true_type {};						

// macro for instantiating the struct SimboxHasMethod##MethodName
#define SIMBOX_HAS_METHOD(MethodName) SimboxHasMethod##MethodName



// macro for name of the struct SimboxFunctorFor##MethodName
#define SIMBOX_FUNCTOR_FOR(FunctionName) SimboxFunctorFor##FunctionName
#define SIMBOX_INSTANTIATE_FUNCTOR_FOR(Name) SIMBOX_FUNCTOR_FOR(Name)()
// macro for defining the struct SimboxFunctorFor##FunctionName
#define SIMBOX_FUNCTOR_FOR_DEF(FunctionName) 									\
	struct SIMBOX_FUNCTOR_FOR(FunctionName) { 									\
		template <typename Iterator,											\
				  typename T = 													\
				  typename std::enable_if<										\
				  		   SIMBOX_HAS_METHOD(FunctionName)<decltype(*std::declval<Iterator>())>::value, 		\
				  		   Iterator 											\
				  		   >::type 												\
				  		   > 													\
		static decltype(auto) get(Iterator & it){return it->FunctionName();}; 	\
																				\
		template <typename Iterator>											\
		decltype(auto) operator()(Iterator & it){return get(it);};				\
	};






					

#define SIMBOX_FUNCTOR_PREPARE(FunctionName, ...)	\
	SIMBOX_HAS_METHOD_DEF(FunctionName);			\
	SIMBOX_FUNCTOR_FOR_DEF(FunctionName);			

























} // end namespace simbox
#endif