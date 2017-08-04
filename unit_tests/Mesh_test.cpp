#include "../include/Mesh.hpp"

#include <iostream>
#include <vector>


int main(int argc, char * argv[]){
	
	std::size_t nvals = 20;
	std::vector<int> v1(nvals);

	struct LineNodePolicy{
		double dx = 0.5;
		double x0 = -1.0;

		double x(std::size_t key) const {return x0+key*dx;}; 
	};
	// struct NoAdapt {};
	typedef simbox::Mesh<simbox::Regular, simbox::Point1Element, simbox::NodeData, LineNodePolicy, decltype(v1)>		MyMesh;
	MyMesh msh;
	msh.resize(nvals);
	for (auto i=0; i<nvals; i++) msh[i] = 100+i;

	for (auto i=0; i<nvals; i++){
		std::cout << "x: " << msh.x(i) << " \tval: " << msh[i] << std::endl;
	}

	
	return 0;
}