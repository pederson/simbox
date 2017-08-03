/** @file DataBufferWriter.hpp
 *  @brief file with DataBufferWriter base
 *
 *  This contains the DataBufferWriter base
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _DATABUFFERWRITER_H
#define _DATABUFFERWRITER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>


// need this to make directories
// but in the future this should use the
// c++17 <filesystem> library
/////////////////////////////////
#include <sys/stat.h>
#include <dirent.h>
/////////////////////////////////

// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;


 namespace simbox{




void make_directory(std::string path){
	mkdir(path.c_str(), 0700);
}

bool check_directory(std::string path){
	if (opendir(path.c_str()) == nullptr) return false;
	return true;
}


/** @class DataBufferWriter
 *  @brief base class for simulationd data I/O
 *
 *  Simulation data base class that defines standard
 *	functions for I/O involving simulations
 *	
 *	BufferPolicy is a container type that acts as a  
 *	data buffer.
 *
 *	WriterPolicy is an I/O type that writes the data
 *	that resides in the buffer to a file
 *
 */
template <class BufferPolicy,
		  class WriterPolicy>
class DataBufferWriter : public BufferPolicy, public WriterPolicy{
public:

	DataBufferWriter(std::string name)
	: WriterPolicy(name)
	, BufferPolicy() {};

	template <typename... Args>
	void write_buffer(Args... args){
		WriterPolicy::write(&*BufferPolicy::begin(), BufferPolicy::size(), args...);
	};


	template <typename... Args>
	void read_buffer(Args... args){
		WriterPolicy::read(&*BufferPolicy::begin(), BufferPolicy::size(), args...);
	};
};




class AsciiFileSystemWriter{
public:
	AsciiFileSystemWriter(std::string foldername)
	: mFolderName(foldername)
	{
		make_directory(mFolderName);
	};

	// here, the Args arguments specify the path to the file
	// in which we want to read/write
	template <typename T, typename... Args>
	bool write(T * data, std::size_t length, Args... args){
		std::ofstream str;
		// std::cout << mFolderName+"/"+getFilepath(mFolderName, args...) << std::endl;
		str.open(mFolderName+"/"+getFilepath(mFolderName, args...), std::ofstream::out | std::ofstream::trunc);
		if (!str.is_open()){
			std::cout << "error opening file for write" << std::endl;
			return false;
		}

		for (auto i=0; i<length; i++){
			str << data[i] << "\n";
		}
		return true;
	}


	template <typename T, typename... Args>
	bool read(T * target, std::size_t length, Args... args){
		std::ifstream str;
		str.open(mFolderName+"/"+getFilepath(mFolderName, args...), std::ifstream::in);
		if (!str.is_open()){
			std::cout << "error opening file for read" << std::endl;
			return false;
		}

		for (auto i=0; i<length; i++){
			str >> target[i];
		}
		return true;
	}

	void close(){};

private:
	std::string 		mFolderName;

	template <typename... Path>
	std::string getFilepath(std::string basefolder, std::string first, Path... pth){
		// std::cout << "checking dir: " << basefolder +"/"+first << std::endl;
		if (!check_directory(basefolder+"/"+first)) make_directory(basefolder+"/"+first);
		return first+"/"+getFilepath(basefolder+"/"+first, pth...);
	}

	// base case
	std::string getFilepath(std::string basefolder, std::string filename){
		return filename;
	}
};





typedef DataBufferWriter<std::vector<double>, AsciiFileSystemWriter> DefaultBufferWriter; 

} // end namespace simbox
#endif