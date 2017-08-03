#include "../include/DataBufferWriter.hpp"

#include <iostream>
#include <vector>


int main(int argc, char * argv[]){
	
	std::vector<int> v1(100);
	for (auto i=0; i<100; i++) v1[i] = i;
	simbox::DefaultBufferWriter bio("test");

	
	bio.resize(100);
	for (auto i=0; i<100; i++) bio[i] = v1[i];
	bio.write_buffer("booger", "eater", "testfile.txt");
	for (auto i=0; i<100; i++) bio[i] = v1[i]+1;
	bio.write_buffer("booger", "picker", "test2.txt");
for (auto i=0; i<100; i++) bio[i] = v1[i]-1;
	bio.write_buffer("doodee", "licker", "test3.txt");
	// for (auto it=zit; it!=zitend; it++){
		
	// }

	return 0;
}