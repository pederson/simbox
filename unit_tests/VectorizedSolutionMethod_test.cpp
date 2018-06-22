#include "../include/VectorizedSolutionMethod.hpp"

#include <iostream>
#include <vector>
#include <functional>


/////// The following is the object that will be held in the container
struct ContainedObject{
private:
	double mA;
	double mB;

public:
	ContainedObject() : mA(0), mB(0.0) {};

	double & a() {return mA;};
	const double & a() const {return mA;};

	double & b() {return mB;};
	const double & b() const {return mB;};
};
////////////////////////////////////////



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

typedef std::vector<ContainedObject> container_type;

template <typename ContainerT, typename Functor>
using VSM = simbox::VectorizedSolutionMethod<ContainerT, Functor>;


typedef std::function<double&(const typename container_type::iterator &)>	functor_type;
std::vector<functor_type> functs = {FunctorForA(),
									FunctorForB()};


template <typename Derived>
struct ContainedObjectVectorizer{
private:
	Derived & derived() {return *static_cast<Derived *>(this);};
	const Derived & derived() const {return *static_cast<const Derived *>(this);};
public:
	VSM<Derived, functor_type> vectorize() {return VSM<Derived, functor_type>(derived(), functs);};
}; 




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





struct ExtendedVector : public std::vector<ContainedObject>, 
						public ContainedObjectVectorizer<ExtendedVector>
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

	std::cout << "vectorized size: " << myvec.vectorize().size() << std::endl;
	std::cout << "container size: " << myvec.size() << std::endl;

	int ct = 0;
	// access a() through the normal container iteration
	for (auto it=myvec.begin(); it!=myvec.end(); it++){
		it->a() = ct; 
		ct++;
	}

	ct=0;
	// access vectorize() through the normal container iteration
	for (auto it=myvec.vectorize().begin(); it!=myvec.vectorize().end(); it++){
		std::cout << *it << std::endl;
		(*it) = ct;
		ct++;
	}

	std::cout << "finished iterating" << std::endl;

	for (auto it=myvec.vectorize().begin(); it!=myvec.vectorize().end(); it++){
		std::cout << *it << std::endl;
	}

	return 0;
}