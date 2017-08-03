#include "../include/DataBufferWriter.hpp"

#include <iostream>
#include <vector>


int main(int argc, char * argv[]){
	
	std::size_t nvals = 20;
	std::vector<int> v1(nvals);
	for (auto i=0; i<nvals; i++) v1[i] = i;
	simbox::DefaultBufferWriter bio("test");
	bio.resize(nvals);
	for (auto i=0; i<nvals; i++) bio[i] = v1[i];

	// test the filesystem write
	bio.write_buffer("booger", "eater", "testfile.txt");
	for (auto i=0; i<nvals; i++) bio[i] = v1[i]+1;
	bio.write_buffer("booger", "picker", "test2.txt");
	for (auto i=0; i<nvals; i++) bio[i] = v1[i]-1;
	bio.write_buffer("doodee", "licker", "test3.txt");


	// test the filesystem read
	bio.read_buffer("booger", "eater", "testfile.txt");
	for (auto i=0; i<nvals; i++) std::cout << bio[i] << std::endl;

	return 0;
}