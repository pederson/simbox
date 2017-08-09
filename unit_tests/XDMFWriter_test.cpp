#include "../include/XDMFWriter.hpp"

#include <iostream>
#include <vector>

// using DI = simbox::DataItem;
int main(int argc, char * argv[]){

	// typedef simbox::DataBufferWriter<std::vector<int>, simbox::HDFFileSystemWriter> Writer;
	typedef simbox::XDMFWriter Writer;
	

	Writer xout("test.xdmf");
	xout.writeHeader("H5FILE", "test.h5", "NUMVERTS", 5);
	xout.writeDataItem<simbox::DataItem::HDF, 
					   simbox::DataItem::Int, 
					   simbox::DataItem::Four>("field1", "5 5", Writer::referenceEntity("H5FILE")+":/here/I/am");
	xout.writeDataItem<simbox::DataItem::HDF, 
					   simbox::DataItem::Float, 
					   simbox::DataItem::Eight>("test1", "1 "+Writer::referenceEntity("NUMVERTS"), Writer::referenceEntity("H5FILE")+":/here/I/am");
	xout.writeDataItemReference("test1");


	xout.write(Writer::dataItem<simbox::DataItem::XML, simbox::DataItem::Four>("/blah/here/me/rawr"));


	xout.close();
	return 0;
}