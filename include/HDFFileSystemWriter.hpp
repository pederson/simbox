/** @file SimulationDataHDF.hpp
 *  @brief file with SimulationDataHDF class
 *
 *  This contains the SimulationDataHDF class
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _HDFFILESYSTEMWRITER_H
#define _HDFFILESYSTEMWRITER_H

#include <string>
#include <vector>
// #include <fstream>
#include <iostream>


// HDF5 library //
#include <hdf5.h>
//////////////////

namespace simbox{



// list out datatypes
template <typename T>
struct H5DataType{
	// throw an error if this is instantiated instead of one of the specializations
	// static_assert(false, "H5DataType must have an overloaded template argument");
	static const hid_t value;
};
template<> const hid_t H5DataType<char>::value 				= H5T_NATIVE_CHAR;
template<> const hid_t H5DataType<signed char>::value 		= H5T_NATIVE_SCHAR;
template<> const hid_t H5DataType<unsigned char>::value 	= H5T_NATIVE_UCHAR;
template<> const hid_t H5DataType<short>::value 			= H5T_NATIVE_SHORT;
template<> const hid_t H5DataType<unsigned short>::value 	= H5T_NATIVE_USHORT;
template<> const hid_t H5DataType<int>::value 				= H5T_NATIVE_INT;
template<> const hid_t H5DataType<unsigned int>::value 		= H5T_NATIVE_UINT;
template<> const hid_t H5DataType<long>::value 				= H5T_NATIVE_LONG;
template<> const hid_t H5DataType<unsigned long>::value 	= H5T_NATIVE_ULONG;
template<> const hid_t H5DataType<long long>::value 		= H5T_NATIVE_LLONG;
template<> const hid_t H5DataType<unsigned long long>::value = H5T_NATIVE_ULLONG;
template<> const hid_t H5DataType<float>::value 			= H5T_NATIVE_FLOAT;
template<> const hid_t H5DataType<double>::value 			= H5T_NATIVE_DOUBLE;
template<> const hid_t H5DataType<long double>::value 		= H5T_NATIVE_LDOUBLE;
// template<> const hid_t H5DataType<hsize_t>::value 			= H5T_NATIVE_HSIZE;
// template<> const hid_t H5DataType<hssize_t>::value 			= H5T_NATIVE_HSSIZE;
// template<> const hid_t H5DataType<herr_t>::value 			= H5T_NATIVE_HERR;
// template<> const hid_t H5DataType<hbool_t>::value 			= H5T_NATIVE_HBOOL;




/** @class HDFFileSystemWriter
 *  @brief class for I/O using HDF5 file format
 *
 *  Use this class to access and write data to/from
 *	the HDF5 file format. Parallel read/write is 
 *	possible with the HDF5 format using MPI
 *
 */
class HDFFileSystemWriter{
public:
	HDFFileSystemWriter(std::string filename)
	: mFileName(filename)
	{
		// collectively open file
		mPlistId = H5Pcreate(H5P_FILE_ACCESS);

		// if MPI enabled...
		// H5Pset_fapl_mpio(mPlistId, MPI_COMM_WORLD, MPI_INFO_NULL);
		

		mH5File = H5Fcreate(mFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, mPlistId);
		H5Pclose(mPlistId);
	};

	// here, the Args arguments specify the path to the file
	// in which we want to read/write
	template <typename T, typename... Args>
	bool write(T * data, hsize_t length, Args... args){
		hid_t ds = makeDataspace(1, &length);
		hid_t set = makeDataset(mH5File, args..., H5DataType<T>::value, ds);
		hid_t ms = makeDataspace(1, &length);

		// get propertylist ID
		mPlistId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(mPlistId, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset = 0, count = length, stride = 1, block = 1;
		H5Sselect_hyperslab(ds, H5S_SELECT_SET, &offset, &stride, &count, &block);
		H5Dwrite(set, H5DataType<T>::value, ms, ds, mPlistId, data);
		
		H5Pclose(mPlistId);
		H5Sclose(ms);
		H5Dclose(set);
		H5Sclose(ds);
		return true;
	}


	// create a dataset without writing anything to it
	template <typename T, typename... Path>
	bool create(hsize_t nrows, hsize_t ncols, Path... pth){
		hsize_t dssize[2]; dssize[0] = nrows; dssize[1] = ncols;
		hid_t ds = makeDataspace(2, dssize);
		hid_t set = makeDataset(mH5File, pth..., H5DataType<T>::value, ds);

		H5Dclose(set);
		H5Sclose(ds);
		return true;
	}


	// FIXME: This is SO SLOW, because of the runtime use of tuples...
	// probably better off by using a pointer with length given
	// write a row of data to an existing dataset
	template <typename Arg1, typename... Args, typename... Path>
	bool write_row(std::size_t row, std::tuple<Arg1, Args...> cols, Path... pth){
		hsize_t length = std::tuple_size<std::tuple<Arg1, Args...>>::value;
		std::vector<Arg1> vals(length);
		auto it = vals.begin();
		Detail::for_each(cols, [&it](auto & r){*it = r; it++;});

		
		hid_t set = openDataset(mH5File, pth...);
		hid_t ds = H5Dget_space(set); 	// this makes a copy
		hid_t ms = makeDataspace(1, &length);

		// get propertylist ID
		mPlistId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(mPlistId, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset[2], count[2], stride[2], block[2];
		offset[0] = row; offset[1] = 0;
		count[0] = 1; count[1] = length;
		stride[0] = 1; stride[1] = 1;
		block[0] = 1; block[1] = 1;
		H5Sselect_hyperslab(ds, H5S_SELECT_SET, offset, stride, count, block);
		H5Dwrite(set, H5DataType<Arg1>::value, ms, ds, mPlistId, &vals.front());
		
		H5Pclose(mPlistId);
		H5Sclose(ms);
		H5Sclose(ds);
		H5Dclose(set);
		return true;
	}

	// write a column of data to an existing dataset
	template <typename T, typename... Path>
	bool write_col(std::size_t col, T * data, hsize_t length, Path... pth){
		
		hid_t set = openDataset(mH5File, pth...);
		hid_t ds = H5Dget_space(set); 	// this makes a copy
		hid_t ms = makeDataspace(1, &length);

		// get propertylist ID
		mPlistId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(mPlistId, H5FD_MPIO_INDEPENDENT);

		// write to file
		hsize_t offset[2], count[2], stride[2], block[2];
		offset[0] = 0; offset[1] = col;
		count[0] = length; count[1] = 1;
		stride[0] = 1; stride[1] = 1;
		block[0] = 1; block[1] = 1;
		H5Sselect_hyperslab(ds, H5S_SELECT_SET, offset, stride, count, block);
		H5Dwrite(set, H5DataType<T>::value, ms, ds, mPlistId, data);
		
		H5Pclose(mPlistId);
		H5Sclose(ms);
		H5Sclose(ds);
		H5Dclose(set);
		return true;
	}


	template <typename T, typename... Args>
	bool read(T * target, hsize_t length, Args... args){
		hid_t ds = makeDataspace(1, &length);
		hid_t set = openDataset(mH5File, args...);
		hid_t ms = makeDataspace(1, &length);

		// get propertylist ID
		mPlistId = H5Pcreate(H5P_DATASET_XFER);
		H5Pset_dxpl_mpio(mPlistId, H5FD_MPIO_INDEPENDENT);

		H5Dread(set, H5DataType<T>::value, ms, ds, mPlistId, target);
		
		H5Pclose(mPlistId);
		H5Sclose(ms);
		H5Dclose(set);
		H5Sclose(ds);
		return true;
	}

	void close(){
		H5Fclose(mH5File);
	};


private:
	std::string 		mFileName;

	hid_t 				mH5File;
	hid_t				mPlistId;






	// make a new dataset at a given location, and return an identifier to the dataset
	template <typename... Path>
	hid_t makeDataset(hid_t folder, std::string first, Path... pth){
		hid_t newfolder;
		if (checkExists(folder, first) > 0) newfolder = H5Gopen(folder, first.c_str(), H5P_DEFAULT);
		else newfolder = makeFolder(folder, first);
		hid_t ds = makeDataset(newfolder, pth...);
		H5Gclose(newfolder);
		return ds;
	}

	// make a new folder at a given location and return an indentifier to the folder
	template <typename... Path>
	hid_t makeFolder(hid_t basefolder, std::string first, Path... pth){
		hid_t newbase;
		if (checkExists(basefolder, first) > 0) newbase = H5Gopen(basefolder, first.c_str(), H5P_DEFAULT);
		else newbase = makeFolder(basefolder, first);
		hid_t fid = makeFolder(newbase, pth...);
		H5Gclose(newbase);
		return fid;
	}


	// open an existing dataset
	// be sure to close it when done!!!
	template <typename... Path>
	hid_t openDataset(hid_t folder, std::string first, Path... pth){
		hid_t newfolder;
		if (checkExists(folder, first) > 0) newfolder = H5Gopen(folder, first.c_str(), H5P_DEFAULT);
		else{
			std::cout << "HDFFileSystemWriter::ERROR folder does not exist in HDF file" << std::endl;
			throw -1;
		}

		hid_t ds = openDataset(newfolder, pth...);
		H5Gclose(newfolder);
		return ds;
	}

	



	///////////// Wrappers for HDF5 functions ///////////////

	// make a new dataset at a given location, and return an identifier to the dataset
	hid_t makeDataset(hid_t folder, std::string dataname, hid_t datatype, hid_t dataspace, 
					  hid_t link_creation_plist_id = H5P_DEFAULT,
					  hid_t group_creation_plist_id = H5P_DEFAULT,
					  hid_t group_access_plist_id = H5P_DEFAULT){
		return H5Dcreate(folder, dataname.c_str(), datatype, dataspace, link_creation_plist_id, group_creation_plist_id, group_access_plist_id);
	}

	// make a folder at a given location, and return an identifier to the new folder
	// this will fail if the folder already exists
	hid_t makeFolder(hid_t folderlocation, std::string foldername, 
					 hid_t link_creation_plist_id = H5P_DEFAULT,
					 hid_t group_creation_plist_id = H5P_DEFAULT,
					 hid_t group_access_plist_id = H5P_DEFAULT)
	{
		return H5Gcreate(folderlocation, foldername.c_str(), link_creation_plist_id, group_creation_plist_id, group_access_plist_id);
	}


	hid_t makeDataspace(int rank, const hsize_t * dims, const hsize_t * maxdims = NULL){
		return H5Screate_simple(rank, dims, maxdims);
	}

	// check if a file or folder exists
	// if > 0, it exists
	// if == 0, it doesn't exist
	// if < 0, there was some other error
	hid_t checkExists(hid_t location, std::string name, hid_t link_access_plist_id = H5P_DEFAULT){
		return H5Lexists(location, name.c_str(), link_access_plist_id);
	}


	// open an existing dataset and return its identifier
	hid_t openDataset(hid_t folder, std::string dataname, hid_t dataset_access_plist_id = H5P_DEFAULT){
		return H5Dopen(folder, dataname.c_str(), dataset_access_plist_id);
	}
};



} // end namespace simbox
#endif
