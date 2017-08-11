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





struct Name : public std::pair<std::string, std::string>{
	Name(std::string name)
	: std::pair<std::string, std::string>("Name",name) {};
};


std::pair<std::string, std::string> Dimensions(){
	return std::make_pair("Dimensions", "");
}

template <typename... Args>
std::pair<std::string, std::string> Dimensions(std::size_t d, Args... args){
	return std::make_pair("Dimensions", std::to_string(d)+" "+Dimensions(args...).second);
}




// class str_const {
// // constexpr string
// private:
// 	const char* const p_;
// 	const std::size_t sz_;
// public:

// 	// ctor
// 	template<std::size_t N>
// 	constexpr str_const(const char(&a)[N]) :
// 	p_(a), sz_(N-1) {};

// 	// []
// 	constexpr char operator[](std::size_t n) {
// 		return n < sz_ ? p_[n] : throw std::out_of_range("");
// 	}
// 	constexpr std::size_t size() { return sz_; } // size()
// };





// template <typename T = unsigned int>
// constexpr T binary_const(
// 	str_const b,
// 	std::size_t n = 0,
// 	T x = 0)
// {
// 	return
// 	n == b.size() ? x :
// 	b[n] == ',' ? binary_const<T>(b, n+1, x) :
// 	b[n] == ' ' ? binary_const<T>(b, n+1, x) :
// 	b[n] == '0' ? binary_const<T>(b, n+1, (x*2)+0) :
// 	b[n] == '1' ? binary_const<T>(b, n+1, (x*2)+1) :
// 	throw std::domain_error("Only '0', '1', ',', and ' ' may be used");
// };





// template <std::size_t N>
// struct Tab{
// 	static const char * value; = "\t"+Tab<N-1>::value;
// };
// template <> const std::string Tab<

// template <> const std::string Tab<0>::value = "\0";
// struct Tab<0>{
// 	static const std::string value = "";
// };



// template <int N>
// struct string_literal{

// };



// // template <std::string name>
// struct Name{
// 	static constexpr const char * label = "Name";
// 	static const char * value;

// 	constexpr Name(const char * name)
// 	: value(name) {};
// 	// static const std::string value = name;
// };

template <int N>
class string_literal
{
	const char (&_lit)[N + 1];
public:
	constexpr string_literal(const char (&lit)[N+1])
	: _lit(lit) {};

	std::string to_string() const{
		return std::string(_lit, _lit+N+1);
	};
};


// template <int N>
// struct NameImpl{
// 	static constexpr string_literal<4> label = "Name";
// 	const char (&value)[N+1];

// 	constexpr NameImpl(const char (&lit)[N+1])
// 	: value(lit) {};
// };

// template <int NP1>
// constexpr auto Name(const char (&lit)[NP1]){
// 	return NameImpl<NP1-1>(lit);
// }

// template <char c1, char... c>
// std::string stringpack()


// template <char... c>
// struct Name{
// 	static const std::string label;
// 	static const std::string value;
// };

// template <char... c> const std::string Name::label = "Name";





// Attribute attributes
namespace Attribute{
	struct AttributeType {static constexpr string_literal<13> label = "AttributeType";};
		struct Scalar : public AttributeType 		{};
		struct Vector : public AttributeType 		{};
		struct Tensor : public AttributeType 		{};
		struct Tensor6 : public AttributeType 		{};
		struct Matrix : public AttributeType 		{};
		struct GlobalID : public AttributeType 		{};

	struct Center {static constexpr string_literal<6> label = "Center";};
		struct Node : public Center 		{};
		struct Cell : public Center 		{};
		struct Grid : public Center 		{};
		struct Face : public Center 		{};
		struct Edge : public Center 		{};
};// end namespace Attribute



// DataItem attributes
namespace DataItem{
	struct ItemType {static constexpr string_literal<8> label = "ItemType";};
		struct Uniform : public ItemType 	{};
		struct Collection : public ItemType {};
		struct Tree : public ItemType 		{};
		struct HyperSlab : public ItemType 	{};
		struct Coordinates : public ItemType{};
		struct Function : public ItemType 	{};


	struct NumberType {static constexpr string_literal<10> label = "NumberType";};
		struct Float : public NumberType 	{};
		struct Int : public NumberType 		{};
		struct UInt : public NumberType 	{};
		struct Char : public NumberType 	{};
		struct UChar : public NumberType 	{};

	struct Precision {static constexpr string_literal<9> label = "Precision";};
		struct One : public Precision 		{};
		struct Two : public Precision 		{};
		struct Four : public Precision 		{};
		struct Eight : public Precision 	{};

	struct Format{static constexpr string_literal<6> label = "Format";};
		struct XML : public Format 			{};
		struct HDF : public Format 			{};
		struct Binary : public Format 		{};

	struct Endian {static constexpr string_literal<6> label = "Endian";};
		struct Native : public Endian 		{};
		struct Big : public Endian 			{};
		struct Little : public Endian 		{};

	struct Compression {static constexpr string_literal<11> label = "Compression";};
		struct Raw : public Compression 	{};
		struct Zlib : public Compression 	{};
		struct BZip2 : public Compression 	{};
};// end namespace DataItem



// Geometry attributes
namespace Geometry{
	struct GeometryType {static constexpr string_literal<12> label = "GeometryType";};
		struct XYZ : public GeometryType 			{};
		struct XY : public GeometryType 			{};
		struct X_Y_Z : public GeometryType 			{};
		struct VxVyVz : public GeometryType 		{};
		struct Origin_DxDyDz : public GeometryType	{};
		struct Origin_DxDy : public GeometryType 	{};
};// end namespace Geometry



// Grid attributes
namespace Grid{
	struct GridType {static constexpr string_literal<8> label = "GridType";};
		struct Uniform : public GridType 	{};
		struct Collection : public GridType {};
		struct Tree : public GridType 		{};
		struct Subset : public GridType 	{};


	struct CollectionType {static constexpr string_literal<14> label = "CollectionType";};
		struct Spatial : public CollectionType 	{};
		struct Temporal : public CollectionType {};

	struct Section {static constexpr string_literal<7> label = "Section";};
		struct DataItem : public Section 	{};
		struct All : public Section 		{};
};// end namespace Grid



// Topology Topologys
namespace Topology{
	struct TopologyType {static constexpr string_literal<12> label = "TopologyType";};
		struct Polyvertex : public TopologyType 		{};
		struct Polyline : public TopologyType 		{};
		struct Polygon : public TopologyType 		{};
		struct Triangle : public TopologyType 		{};
		struct Quadrilateral : public TopologyType 		{};
		struct Tetrahedron : public TopologyType 		{};
		struct Pyramid : public TopologyType 		{};
		struct Wedge : public TopologyType 		{};
		struct Hexahedron : public TopologyType 		{};
		struct Edge_3 : public TopologyType 		{};
		struct Triangle_6 : public TopologyType 		{};
		struct Quadrilateral_8 : public TopologyType 		{};
		struct Tetrahedron_10 : public TopologyType 		{};
		struct Pyramid_13 : public TopologyType 		{};
		struct Wedge_15 : public TopologyType 		{};
		struct Hexahedron_20 : public TopologyType 		{};
		struct Mixed : public TopologyType 		{};
		struct SMesh2D : public TopologyType 		{};
		struct RectMesh2D : public TopologyType 		{};
		struct CoRectMesh2D : public TopologyType 		{};
		struct SMesh3D : public TopologyType 		{};
		struct RectMesh3D : public TopologyType 		{};
		struct CoRectMesh3D : public TopologyType 		{};

};// end namespace Topology




// Time attributes
namespace Time{
	struct TimeType {static constexpr string_literal<8> label = "TimeType";};
		struct Single : public TimeType 	{};
		struct HyperSlab : public TimeType 	{};
		struct List : public TimeType 		{};
		struct Range : public TimeType 		{};
};// end namespace Time



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
template <> const std::string Stringify<Attribute::Node>::value 		= "Node";
template <> const std::string Stringify<Attribute::Cell>::value 		= "Cell";
template <> const std::string Stringify<Attribute::Grid>::value 		= "Grid";
template <> const std::string Stringify<Attribute::Face>::value 		= "Face";
template <> const std::string Stringify<Attribute::Edge>::value 		= "Edge";


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




template <> const std::string Stringify<Geometry::XYZ>::label 		= "GeometryType";
template <> const std::string Stringify<Geometry::XY>::label 		= "GeometryType";
template <> const std::string Stringify<Geometry::X_Y_Z>::label 		= "GeometryType";
template <> const std::string Stringify<Geometry::VxVyVz>::label 		= "GeometryType";
template <> const std::string Stringify<Geometry::Origin_DxDyDz>::label 		= "GeometryType";
template <> const std::string Stringify<Geometry::Origin_DxDy>::label 	= "GeometryType";
template <> const std::string Stringify<Geometry::XYZ>::value 		= "XYZ";
template <> const std::string Stringify<Geometry::XY>::value 		= "XY";
template <> const std::string Stringify<Geometry::X_Y_Z>::value 		= "X_Y_Z";
template <> const std::string Stringify<Geometry::VxVyVz>::value 		= "VxVyVz";
template <> const std::string Stringify<Geometry::Origin_DxDyDz>::value 		= "Origin_DxDyDz";
template <> const std::string Stringify<Geometry::Origin_DxDy>::value 	= "Origin_DxDy";




template <> const std::string Stringify<Grid::Uniform>::label 		= "GridType";
template <> const std::string Stringify<Grid::Collection>::label 		= "GridType";
template <> const std::string Stringify<Grid::Tree>::label 		= "GridType";
template <> const std::string Stringify<Grid::Subset>::label 		= "GridType";
template <> const std::string Stringify<Grid::Uniform>::value 		= "Uniform";
template <> const std::string Stringify<Grid::Collection>::value 		= "Collection";
template <> const std::string Stringify<Grid::Tree>::value 		= "Tree";
template <> const std::string Stringify<Grid::Subset>::value 		= "Subset";

template <> const std::string Stringify<Grid::Spatial>::label 		= "CollectionType";
template <> const std::string Stringify<Grid::Temporal>::label 		= "CollectionType";
template <> const std::string Stringify<Grid::Spatial>::value 		= "Spatial";
template <> const std::string Stringify<Grid::Temporal>::value 		= "Temporal";

template <> const std::string Stringify<Grid::DataItem>::label 		= "Section";
template <> const std::string Stringify<Grid::All>::label 		= "Section";
template <> const std::string Stringify<Grid::DataItem>::value 		= "DataItem";
template <> const std::string Stringify<Grid::All>::value 		= "All";



template <> const std::string Stringify<Topology::Polyvertex>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Polyline>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Polygon>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Triangle>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Quadrilateral>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Tetrahedron>::label 	= "TopologyType";
template <> const std::string Stringify<Topology::Pyramid>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Wedge>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Hexahedron>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Edge_3>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Triangle_6>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Quadrilateral_8>::label 	= "TopologyType";
template <> const std::string Stringify<Topology::Tetrahedron_10>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Pyramid_13>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Wedge_15>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Hexahedron_20>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::Mixed>::label 	= "TopologyType";
template <> const std::string Stringify<Topology::SMesh2D>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::RectMesh2D>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::CoRectMesh2D>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::SMesh3D>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::RectMesh3D>::label 		= "TopologyType";
template <> const std::string Stringify<Topology::CoRectMesh3D>::label 	= "TopologyType";

template <> const std::string Stringify<Topology::Polyvertex>::value 		= "Polyvertex";
template <> const std::string Stringify<Topology::Polyline>::value 		= "Polyline";
template <> const std::string Stringify<Topology::Polygon>::value 		= "Polygon";
template <> const std::string Stringify<Topology::Triangle>::value 		= "Triangle";
template <> const std::string Stringify<Topology::Quadrilateral>::value 		= "Quadrilateral";
template <> const std::string Stringify<Topology::Tetrahedron>::value 	= "Tetrahedron";
template <> const std::string Stringify<Topology::Pyramid>::value 		= "Pyramid";
template <> const std::string Stringify<Topology::Wedge>::value 		= "Wedge";
template <> const std::string Stringify<Topology::Hexahedron>::value 		= "Hexahedron";
template <> const std::string Stringify<Topology::Edge_3>::value 		= "Edge_3";
template <> const std::string Stringify<Topology::Triangle_6>::value 		= "Triangle_6";
template <> const std::string Stringify<Topology::Quadrilateral_8>::value 	= "Quadrilateral_8";
template <> const std::string Stringify<Topology::Tetrahedron_10>::value 		= "Tetrahedron_10";
template <> const std::string Stringify<Topology::Pyramid_13>::value 		= "Pyramid_13";
template <> const std::string Stringify<Topology::Wedge_15>::value 		= "Wedge_15";
template <> const std::string Stringify<Topology::Hexahedron_20>::value 		= "Hexahedron_20";
template <> const std::string Stringify<Topology::Mixed>::value 	= "Mixed";
template <> const std::string Stringify<Topology::SMesh2D>::value 		= "SMesh2D";
template <> const std::string Stringify<Topology::RectMesh2D>::value 		= "RectMesh2D";
template <> const std::string Stringify<Topology::CoRectMesh2D>::value 		= "CoRectMesh2D";
template <> const std::string Stringify<Topology::SMesh3D>::value 		= "SMesh3D";
template <> const std::string Stringify<Topology::RectMesh3D>::value 		= "RectMesh3D";
template <> const std::string Stringify<Topology::CoRectMesh3D>::value 	= "CoRectMesh3D";



template <> const std::string Stringify<Time::Single>::label 		= "TimeType";
template <> const std::string Stringify<Time::HyperSlab>::label 		= "TimeType";
template <> const std::string Stringify<Time::List>::label 		= "TimeType";
template <> const std::string Stringify<Time::Range>::label 		= "TimeType";
template <> const std::string Stringify<Time::Single>::value 		= "Single";
template <> const std::string Stringify<Time::HyperSlab>::value 		= "HyperSlab";
template <> const std::string Stringify<Time::List>::value 		= "List";
template <> const std::string Stringify<Time::Range>::value 		= "Range";




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
	};


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
	};


	bool writeDataItemReference(std::string name){
		return write(dataItemReference(name));
	};


	bool write(std::string content){
		mFileStream << content;
		return true;
	};


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
	};

	static std::string dataItemReference(std::string dataitem){
		return "<DataItem Reference=\"XML\">\n\t/Xdmf/DataItem[@Name=\""+dataitem+"\"]\n"+"</DataItem>\n";
	};

	template <typename FirstAttrib, typename... Attribs>
	static std::string dataItem(std::string content){
		return openBrackets<true, FirstAttrib, Attribs...>("DataItem")+"\n"+content+"\n"+closeBrackets("DataItem");
	};

	// write a dataItem with a name and dimensions specified as strings
	template <typename FirstAttrib, typename... Attribs>
	static std::string dataItem(std::string name, std::string dims, std::string content){
		return openBrackets<true, FirstAttrib, Attribs...>("DataItem Name=\""+name+"\" Dimensions=\""+dims+"\"")
				+"\n"+content+"\n"
				+closeBrackets("DataItem");
	};







	// static basic functions

	template <bool isfirst, typename Attrib1, typename... Attribs>
	static std::string openBrackets(std::string name){
		std::string out;
		if (isfirst) out = "<"+name;
		else out = "";

		// add the attribute
		out = out + " " + Stringify<Attrib1>::label + "=" + "\"" + Stringify<Attrib1>::value + "\"";

		// add the rest of the attributes
		out = out + openBrackets<false, Attribs...>("");
		return out;
	};


	// base case
	template <bool isfirst>
	static std::string openBrackets(std::string name){
		std::string out;
		if (isfirst) out = "<"+name;
		else out = name;

		return out+">";
	};


	template <bool isfirst, typename... Attribs, typename... StringPair>
	static std::string openBrackets(std::string name, std::pair<std::string, std::string> prop1, StringPair... props){
		std::string out;
		if (isfirst) out = "<"+name;
		else out = "";

		// add the property
		out = out + " " + prop1.first + "=" + "\"" + prop1.second + "\"";

		// add the rest of the attributes
		out = out + openBrackets<false, Attribs...>(name, props...);
		return out;
	};


	static std::string closeBrackets(std::string name){
		return "\n</"+name+">\n";
	};


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
	};

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
	};




	
};


} // end namespace simbox

#endif