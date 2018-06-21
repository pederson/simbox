#include "../include/VectorizedSolutionMethod.hpp"

#include <iostream>
#include <vector>


/////// The following is the object that will be held in the container
struct ContainedObject{
private:
	int mA;
	double mB;

public:
	ContainedObject() : mA(0), mB(0.0) {};

	int & a() {return mA;};
	const int & a() const {return mA;};

	double & b() {return mB;};
	const double & b() const {return mB;};
};
////////////////////////////////////////



// // some preprocessor magic to get the names of the functors automatically
// #define FunctorFor(FunctionName) FunctorFor##FunctionName
// #define DefineFunctorFor(FunctionName) \
// 	struct FunctorFor##FunctionName { \
// 		template <typename Iterator> \
// 		static decltype(auto) get(Iterator & it){return it->FunctionName();}; \
// 	};

// DefineFunctorFor(a);

/////// The following are functors used to access each containedobject member
/////// from an iterator
struct FunctorForA{
	template <typename Iterator>
	static decltype(auto) get(Iterator & it){return it->a();};

	template <typename Iterator>
	decltype(auto) operator()(Iterator & it){return get(it);};
};
struct FunctorForB{
	template <typename Iterator>
	static decltype(auto) get(Iterator & it){return it->b();};

	template <typename Iterator>
	decltype(auto) operator()(Iterator & it){return get(it);};
};
///////////////////////////////////////////


template <typename ContainerT, typename Functor>
using VSM = simbox::VectorizedSolutionMethod<ContainerT, Functor>;


// template <typename ContainerT, typename Functor>
// using VCM = simbox::VectorizeContainerMethod<ContainerT, Functor>;


// #define DefineVectorizedFunctor(FunctionName) \
// 	DefineFunctorFor(FunctionName); \
// 	template <typename Derived> \
// 	struct ContainedObjectFunctors{ \
// 	private: \
// 		Derived & derived() {return *static_cast<Derived *>(this);}; \
// 		const Derived & derived() const {return *static_cast<const Derived *>(this);}; \
// 	public: \
// 		VCM<Derived, FunctorFor(FunctionName)> FunctionName() {return VCM<Derived, FunctorFor(FunctionName)>(derived());}; \
// 		VCM<Derived, FunctorForB> b() {return VCM<Derived, FunctorForB>(derived());}; \
// 	}; 

// DefineVectorizedFunctor(a);


// // use CRTP to extend functionality to use VectorizedContainerMethods
// // on classes that implement ContainedObjectFunctors
// template <typename Derived>
// struct ContainedObjectFunctors{
// private:
// 	Derived & derived() {return *static_cast<Derived *>(this);};
// 	const Derived & derived() const {return *static_cast<const Derived *>(this);};
// public:
// 	VCM<Derived, FunctorFor(a)> a() {return VCM<Derived, FunctorFor(a)>(derived());};
// 	VCM<Derived, FunctorForB> b() {return VCM<Derived, FunctorForB>(derived());};
// };



struct ExtendedVector : public std::vector<ContainedObject>
{

	typedef std::vector<ContainedObject> base_type;

	// use base type constructors and iteration accessors
	using base_type::base_type;
	using base_type::begin;
	using base_type::end;
	using base_type::cbegin;
	using base_type::cend;

	// vectorized accessors to individual member data as a vector
	// come from the base class ContainedObjectFunctors
};


int main(int argc, char * argv[]){
	ExtendedVector myvec(5);

	// std::cout << "a size: " << myvec.a().size() << std::endl;
	// std::cout << "b size: " << myvec.b().size() << std::endl;
	std::cout << "container size: " << myvec.size() << std::endl;

	int ct = 0;
	// access a() through the normal container iteration
	for (auto it=myvec.begin(); it!=myvec.end(); it++){
		it->a() = ct; 
		ct++;
	}

	// // access a() through the vectorized function object
	// std::cout << "a: " << std::endl;
	// for (auto it=myvec.a().begin(); it!=myvec.a().end(); it++){
	// 	std::cout << *it << std::endl;
	// }

	// for (auto it=myvec.a().begin(); it!=myvec.a().end(); it++){
	// 	(*it) = 10;//std::cout << *it << std::endl;
	// }

	// std::cout << "a: " << std::endl;
	// for (auto it=myvec.a().begin(); it!=myvec.a().end(); it++){
	// 	std::cout << *it << std::endl;
	// }

	// for (auto it=myvec.b().begin(); it!=myvec.b().end(); it++){
	// 	(*it) = -1.0;//std::cout << *it << std::endl;
	// }

	// std::cout << "b: " << std::endl;
	// for (auto it=myvec.b().begin(); it!=myvec.b().end(); it++){
	// 	std::cout << *it << std::endl;
	// }

	return 0;
}