#include "../include/XDMFWriter.hpp"

#include <iostream>
#include <vector>

// using DI = simbox::DataItem;
int main(int argc, char * argv[]){

	// typedef simbox::DataBufferWriter<std::vector<int>, simbox::HDFFileSystemWriter> Writer;
	typedef simbox::XDMFWriter Writer;

	std::cout << simbox::Stringify<simbox::Attribute::Scalar>::label << std::endl;
	// auto r = simbox::Name("brianwilson");
	

	Writer xout("test.xdmf");
	xout.writeHeader("H5FILE", "test.h5", "NUMVERTS", 5);
	xout.writeDataItem<simbox::DataItem::HDF, 
					   simbox::DataItem::Int, 
					   simbox::DataItem::Four>("field1", "5 5", Writer::referenceEntity("H5FILE")+":/here/I/am");
	xout.writeDataItem<simbox::DataItem::HDF, 
					   simbox::DataItem::Float, 
					   simbox::DataItem::Eight>("test1", "1 "+Writer::referenceEntity("NUMVERTS"), 
					   							Writer::referenceEntity("H5FILE")+":/here/I/am");
	xout.writeDataItemReference("test1");


	xout.write(Writer::dataItem<simbox::DataItem::XML, simbox::DataItem::Four>("rawr","1 10", "/blah/here/me/rawr"));
	xout.write(Writer::openBrackets<true, simbox::DataItem::XML>("Attribute",simbox::Name("namehere2"), simbox::Dimensions(10, 20, 30)));

	xout.close();
	return 0;
}