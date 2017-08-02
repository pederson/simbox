#include "../include/DataBufferWriter.hpp"

#include <iostream>
#include <vector>


int main(int argc, char * argv[]){
	
	std::vector<int> v1(100);
	for (auto i=0; i<100; i++) v1[i] = i;
	simbox::DefaultBufferWriter bio("test");

	
	bio.resize(100);
	for (auto i=0; i<100; i++) bio[i] = v1[i];
	bio.write_buffer("testfile.txt");
	// for (auto it=zit; it!=zitend; it++){
		
	// }

	return 0;
}