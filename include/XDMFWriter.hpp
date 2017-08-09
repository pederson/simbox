/** @file XDMFWriter.hpp
 *  @brief file with XDMFWriter class
 *
 *  This contains the XDMFWriter class
 *  class and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _XDMFWRITER_H
#define _XDMFWRITER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>


namespace simbox{


// Attribute attributes
namespace Attribute{
	struct AttributeType {};
		struct Scalar : public AttributeType 		{};
		struct Vector : public AttributeType 		{};
		struct Tensor : public AttributeType 		{};
		struct Tensor6 : public AttributeType 		{};
		struct Matrix : public AttributeType 		{};
		struct GlobalID : public AttributeType 		{};

	struct Center {};
		struct Node : public Center 		{};
		struct Cell : public Center 		{};
		struct Grid : public Center 		{};
		struct Face : public Center 		{};
		struct Edge : public Center 		{};
}// end namespace Attribute



// DataItem attributes
namespace DataItem{
	struct ItemType {};
		struct Uniform : public ItemType 	{};
		struct Collection : public ItemType {};
		struct Tree : public ItemType 		{};
		struct HyperSlab : public ItemType 	{};
		struct Coordinates : public ItemType{};
		struct Function : public ItemType 	{};


	struct NumberType {};
		struct Float : public NumberType 	{};
		struct Int : public NumberType 		{};
		struct UInt : public NumberType 	{};
		struct Char : public NumberType 	{};
		struct UChar : public NumberType 	{};

	struct Precision {};
		struct One : public Precision 		{};
		struct Two : public Precision 		{};
		struct Four : public Precision 		{};
		struct Eight : public Precision 	{};

	struct Format{};
		struct XML : public Format 			{};
		struct HDF : public Format 			{};
		struct Binary : public Format 		{};

	struct Endian {};
		struct Native : public Endian 		{};
		struct Big : public Endian 			{};
		struct Little : public Endian 		{};

	struct Compression {};
		struct Raw : public Compression 	{};
		struct Zlib : public Compression 	{};
		struct BZip2 : public Compression 	{};
}// end namespace DataItem



// Geometry attributes
namespace Geometry{
	struct GeometryType {};
		struct XYZ : public GeometryType 			{};
		struct XY : public GeometryType 			{};
		struct X_Y_Z : public GeometryType 			{};
		struct VxVyVz : public GeometryType 		{};
		struct Origin_DxDyDz : public GeometryType	{};
		struct Origin_DxDy : public GeometryType 	{};
}// end namespace Geometry



// Grid attributes
namespace Grid{
	struct GridType {};
		struct Uniform : public GridType 	{};
		struct Collection : public GridType {};
		struct Tree : public GridType 		{};
		struct Subset : public GridType 	{};


	struct CollectionType {};
		struct Spatial : public CollectionType 	{};
		struct Temporal : public CollectionType {};

	struct Section {};
		struct DataItem : public Section 	{};
		struct All : public Section 		{};
}// end namespace Grid



// Time attributes
namespace Time{
	struct TimeType {};
		struct Single : public TimeType 	{};
		struct HyperSlab : public TimeType 	{};
		struct List : public TimeType 		{};
		struct Range : public TimeType 		{};
}// end namespace Time



template <typename T>
struct Stringify{static const std::string value; static const std::string label;};

template <> const std::string Stringify<Attribute::Scalar>::label 		= "AttributeType";
template <> const std::string Stringify<Attribute::Vector>::label 		= "AttributeType";
template <> const std::string Stringify<Attribute::Tensor>::label 		= "AttributeType";
template <> const std::string Stringify<Attribute::Tensor6>::label 		= "AttributeType";
template <> const std::string Stringify<Attribute::Matrix>::label 		= "AttributeType";
template <> const std::string Stringify<Attribute::GlobalID>::label 	= "AttributeType";
template <> const std::string Stringify<Attribute::Scalar>::value 		= "Scalar";
template <> const std::string Stringify<Attribute::Vector>::value 		= "Vector";
template <> const std::string Stringify<Attribute::Tensor>::value 		= "Tensor";
template <> const std::string Stringify<Attribute::Tensor6>::value 		= "Tensor6";
template <> const std::string Stringify<Attribute::Matrix>::value 		= "Matrix";
template <> const std::string Stringify<Attribute::GlobalID>::value 	= "GlobalID";

template <> const std::string Stringify<Attribute::Node>::label 		= "Center";
template <> const std::string Stringify<Attribute::Cell>::label 		= "Center";
template <> const std::string Stringify<Attribute::Grid>::label 		= "Center";
template <> const std::string Stringify<Attribute::Face>::label 		= "Center";
template <> const std::string Stringify<Attribute::Edge>::label 		= "Center";
template <> const std::string Stringify<Attribute::Node>::label 		= "Node";
template <> const std::string Stringify<Attribute::Cell>::label 		= "Cell";
template <> const std::string Stringify<Attribute::Grid>::label 		= "Grid";
template <> const std::string Stringify<Attribute::Face>::label 		= "Face";
template <> const std::string Stringify<Attribute::Edge>::label 		= "Edge";


template <> const std::string Stringify<DataItem::Float>::label 	= "NumberType";
template <> const std::string Stringify<DataItem::Int>::label 		= "NumberType";
template <> const std::string Stringify<DataItem::UInt>::label 		= "NumberType";
template <> const std::string Stringify<DataItem::Char>::label 		= "NumberType";
template <> const std::string Stringify<DataItem::UChar>::label 	= "NumberType";
template <> const std::string Stringify<DataItem::Float>::value 	= "Float";
template <> const std::string Stringify<DataItem::Int>::value 		= "Int";
template <> const std::string Stringify<DataItem::UInt>::value 		= "UInt";
template <> const std::string Stringify<DataItem::Char>::value 		= "Char";
template <> const std::string Stringify<DataItem::UChar>::value 	= "UChar";

template <> const std::string Stringify<DataItem::One>::label 		= "Precision";
template <> const std::string Stringify<DataItem::Two>::label 		= "Precision";
template <> const std::string Stringify<DataItem::Four>::label 		= "Precision";
template <> const std::string Stringify<DataItem::Eight>::label 	= "Precision";
template <> const std::string Stringify<DataItem::One>::value 		= "One";
template <> const std::string Stringify<DataItem::Two>::value 		= "Two";
template <> const std::string Stringify<DataItem::Four>::value 		= "Four";
template <> const std::string Stringify<DataItem::Eight>::value 	= "Eight";

template <> const std::string Stringify<DataItem::XML>::label 		= "Format";
template <> const std::string Stringify<DataItem::HDF>::label 		= "Format";
template <> const std::string Stringify<DataItem::Binary>::label 	= "Format";
template <> const std::string Stringify<DataItem::XML>::value 		= "XML";
template <> const std::string Stringify<DataItem::HDF>::value 		= "HDF";
template <> const std::string Stringify<DataItem::Binary>::value 	= "Binary";

template <> const std::string Stringify<DataItem::Native>::label 	= "Endian";
template <> const std::string Stringify<DataItem::Big>::label 		= "Endian";
template <> const std::string Stringify<DataItem::Little>::label 	= "Endian";
template <> const std::string Stringify<DataItem::Native>::value 	= "Native";
template <> const std::string Stringify<DataItem::Big>::value 		= "Big";
template <> const std::string Stringify<DataItem::Little>::value 	= "Little";

template <> const std::string Stringify<DataItem::Raw>::label 		= "Compression";
template <> const std::string Stringify<DataItem::Zlib>::label 		= "Compression";
template <> const std::string Stringify<DataItem::BZip2>::label 	= "Compression";
template <> const std::string Stringify<DataItem::Raw>::value 		= "Raw";
template <> const std::string Stringify<DataItem::Zlib>::value 		= "Zlib";
template <> const std::string Stringify<DataItem::BZip2>::value 	= "BZip2";



/** @class XDMFWriter
 *  @brief class for I/O using XDMF markup file format
 *
 *  Use this class to access and write data to/from
 *	the XDMF file format.
 *
 */
class XDMFWriter{
public:
	XDMFWriter(std::string filename)
	: mFileName(filename)
	{
		mFileStream.open(filename, std::ofstream::out | std::ofstream::trunc);
	};

	// here, the Args arguments are pairs that specify entity names and entity values
	// the entvalue type must be convertible to string
	template <typename entvalue, typename... Args>
	bool writeHeader(std::string firstentity, entvalue firstvalue, Args... args){
		return writeHeaderInternal(true, firstentity, firstvalue, args...);
	}


	template <typename DataItemFormat,
			  typename DataItemNumberType,
			  typename DataItemPrecision>
	bool writeDataItem(std::string name, 
					   std::string dims, 
					   std::string path){
		static_assert(std::is_base_of<DataItem::Format, DataItemFormat>::value, "XDMF:: Must be an existing Format");
		static_assert(std::is_base_of<DataItem::NumberType, DataItemNumberType>::value, "XDMF:: Must be an existing NumberType");
		static_assert(std::is_base_of<DataItem::Precision, DataItemPrecision>::value, "XDMF:: Must be an existing Precision");


		// mFileStream << "\n<!--===================== Data Item ====================-->" << std::endl;
		mFileStream << "<DataItem Name=\"" << name ;
		mFileStream << "\" Format=\"" << Stringify<DataItemFormat>::value ;
		mFileStream << "\" NumberType=\"" << Stringify<DataItemNumberType>::value ;
		mFileStream << "\" Precision=\"" << Stringify<DataItemPrecision>::value ;
		mFileStream << "\" Dimensions=\"" << dims ; 
		mFileStream << "\">" << std::endl;
		mFileStream << "\t" << path << std::endl;
		mFileStream << "</DataItem>" << std::endl;
		return true;
	}


	bool writeDataItemReference(std::string name){
		return write(dataItemReference(name));
	}


	bool write(std::string content){
		mFileStream << content;
		return true;
	}


	// template <typename T, typename... Args>
	// bool read(Args... args){

	// }

	void close(){
		mFileStream << "\n</Xdmf>" ;
		mFileStream.close();
	};


	// // here, the Args arguments are pairs that specify entity names and entity values
	// // the entvalue type must be convertible to string
	// template <typename entvalue, typename... Args>
	// static std::string header(std::string firstentity, entvalue firstvalue, Args... args){
	// 	return writeHeaderInternal(true, firstentity, firstvalue, args...);
	// }

	// some static functions that return a string
	static std::string referenceEntity(std::string entityname){
		return "&"+entityname+";";
	}

	static std::string dataItemReference(std::string dataitem){
		return "<DataItem Reference=\"XML\">\n\t/Xdmf/DataItem[@Name=\""+dataitem+"\"]\n"+"</DataItem>\n";
	}

	template <typename FirstAttrib, typename... Attribs>
	static std::string dataItem(std::string content){
		return openBrackets<true, FirstAttrib, Attribs...>("DataItem")+"\n"+content+"\n"+closeBrackets("DataItem");
	}


private:
	std::string 		mFileName;
	std::ofstream 		mFileStream;

	// here, the Args arguments are pairs that specify entity names and entity values
	// the entvalue type must be convertible to string
	template <typename entvalue, typename... Args>
	bool writeHeaderInternal(bool dobegin, std::string firstentity, entvalue firstvalue, Args... args){
		if (dobegin){
			mFileStream << "<?xml version=\"1.0\" ?>" << std::endl;
			mFileStream << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" [" << std::endl;
		}
		mFileStream << "\t<!ENTITY " << firstentity << " \"" << firstvalue << "\">" << std::endl;
		return writeHeaderInternal(false, args...);
	}

	// base case
	bool writeHeaderInternal(bool dobegin){
		if (dobegin){
			mFileStream << "<?xml version=\"1.0\" ?>" << std::endl;
			mFileStream << "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" [" << std::endl;
		}

		// close out header
		mFileStream << "]>" << std::endl;

		// write the version and url tag
		mFileStream << "<Xdmf Version=\"2.0\" xmlns:xi=\"http://www.w3.org/2001/XInclude\">" << std::endl;
		return true;
	}




	// static private functions

	template <bool isfirst, typename Attrib1, typename... Attribs>
	static std::string openBrackets(std::string name){
		std::string out;
		if (isfirst) out = "<"+name;
		else out = name;

		// add the attribute
		out = out + " " + Stringify<Attrib1>::label + "=" + "\"" + Stringify<Attrib1>::value + "\"";

		// add the rest of the attributes
		out = out + openBrackets<false, Attribs...>("");
		return out;
	}

	// base case
	template <bool isfirst>
	static std::string openBrackets(std::string name){
		std::string out;
		if (isfirst) out = "<"+name;
		else out = name;

		return out+">";
	}


	static std::string closeBrackets(std::string name){
		return "</"+name+">";
	}

};



} // end namespace simbox
#endif
