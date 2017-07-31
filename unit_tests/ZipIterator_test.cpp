#include "../include/ZipIterator.hpp"



#include <tuple>
#include <iostream>
#include <vector>


int main(int argc, char * argv[]){
	
	std::vector<int> v1(10);
	std::vector<int> v2(10);

	int val=0;
	auto zit = simbox::make_zip_iterator(std::make_tuple(v1.begin(), v2.begin()));
	auto zitend = simbox::make_zip_iterator(std::make_tuple(v1.end(), v2.end()));

	for (auto it=zit; it!=zitend; it++){
		if (it == zit) std::cout << "BEGIN****" << std::endl;

		it.get<0>() = val;
		it.get<1>() = val+1;

		std::cout << "i: " << val ;
		std::cout << " first: " << it.get<0>() ;
		std::cout << " second: " << it.get<1>() ;
		std::cout  << std::endl;
		
		val++;
	}

	return 0;
}