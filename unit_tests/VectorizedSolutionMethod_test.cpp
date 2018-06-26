#include "../include/VectorizeContainerMultifunctor.hpp"
#include "../include/VectorizeContainerFunctor.hpp"

#include <iostream>
#include <vector>
#include <functional>


/////// The following is the object that will be held in the container
struct ContainedObject{
private:
	double mA;
	double mB;
	double mC;

public:
	ContainedObject() : mA(0), mB(0.0), mC(-1.0) {};

	double & a() {return mA;};
	const double & a() const {return mA;};

	double & b() {return mB;};
	const double & b() const {return mB;};

	double & c() {return mC;};
	const double & c() const {return mC;};
};
////////////////////////////////////////


typedef std::vector<ContainedObject> container_type;
typedef std::function<double&(const typename container_type::iterator &)>	functor_type;


// instantiate a vectorization of function a()
// SIMBOX_FOR_EACH_SEP(SIMBOX_VECTORIZE_FUNCTOR_DEF, a, b, c);
SIMBOX_VECTORIZE_FUNCTOR_DEF(a);
SIMBOX_VECTORIZE_FUNCTOR_DEF(b);
SIMBOX_VECTORIZE_FUNCTOR_DEF(c);
SIMBOX_VECTORIZE_MULTIFUNCTOR_DEF(vectorize, functor_type, a, b, c);
SIMBOX_VECTORIZE_MULTIFUNCTOR_DEF(vectorize2, functor_type, b, c);



struct ExtendedVector : public container_type,
						public SIMBOX_VECTORIZE_FUNCTOR(a)<ExtendedVector>,
						public SIMBOX_VECTORIZE_FUNCTOR(c)<ExtendedVector>,
						public SIMBOX_VECTORIZE_MULTIFUNCTOR(vectorize2)<ExtendedVector>,
						public SIMBOX_VECTORIZE_MULTIFUNCTOR(vectorize)<ExtendedVector>
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


	std::cout << "\n\na: " << std::endl;
	for (auto it=myvec.a().begin(); it!=myvec.a().end(); it++){
		std::cout << *it << std::endl;
	}

	std::cout << "\n\nc: " << std::endl;
	for (auto it=myvec.c().begin(); it!=myvec.c().end(); it++){
		std::cout << *it << std::endl;
	}


	std::cout << "vectorize2" << std::endl;
	for (auto it=myvec.vectorize2().cbegin(); it!=myvec.vectorize2().cend(); it++){
		*it = 1;
		std::cout << *it << std::endl;
	}


	return 0;
}