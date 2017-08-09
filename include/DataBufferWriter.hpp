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




 namespace Detail{
	// compile-time for_each on a tuple
	template <typename TupleType, typename FunctionType>
	void for_each(TupleType&&, FunctionType
	            , std::integral_constant<std::size_t, std::tuple_size<typename std::remove_reference<TupleType>::type >::value>) {}

	template <std::size_t I, typename TupleType, typename FunctionType
	       , typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type >
	void for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
	{
	    f(std::get<I>(t));
	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
	}

	template <typename TupleType, typename FunctionType>
	void for_each(TupleType&& t, FunctionType f)
	{
	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
	}
}




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



/** @class AsciiFileSystemWriter
 *  @brief class to write txt files into a foldered system
 *
 *  write arguments are consecutive strings that specify 
 * 	a folder path, ending in a filename
 *
 */
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
		std::cout << mFolderName+"/"+getFilepath(mFolderName, args...) << std::endl;
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


	// append a line of data to a new or existing text file
	template <typename... Args, typename... Path>
	bool append_line(std::tuple<Args...> cols, Path... pth){
		std::ofstream str;
		str.open(mFolderName+"/"+getFilepath(mFolderName, pth...), std::ofstream::out | std::ofstream::app);
		Detail::for_each(cols, [&str](auto & r){str << r << " " ;});
		str << std::endl;
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




// define a default as using a std::vector and AsciiFileSystemWriter
template <typename T>
struct DefaultBufferWriterTypedef{
	typedef DataBufferWriter<std::vector<T>, AsciiFileSystemWriter> type;
};
template <typename T>
using DefaultBufferWriter = typename DefaultBufferWriterTypedef<T>::type;


} // end namespace simbox
#endif