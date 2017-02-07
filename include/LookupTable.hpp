/** @file LookupTable.hpp
 *  @brief Class for LookupTable files
 *
 *  This contains the LookupTable class
 *	and associated functions, defs, and enums
 *
 *  @author Dylan Pederson
 *  @bug No known bugs.
 */

#ifndef _LOOKUPTABLE_H
#define _LOOKUPTABLE_H

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

enum class LookupMode : unsigned char {ASCII,
									   BINARY_DOUBLE,
									   BINARY_FLOAT,
									   BINARY_INT,
									   BINARY_UINT};

enum class StorageMode : unsigned char {ROW_MAJOR,
										COLUMN_MAJOR};


/** @class LookupTable
 *  @brief class for a lookup table file
 *
 *	A class implementing a 
 *	lookup table interface to a table
 *	contained in an ascii or binary file
 *
 */
class LookupTable{
public:
	LookupTable();

	LookupTable(std::string filename, LookupMode lmode, StorageMode smode,
				unsigned int rowstart, unsigned int colstart,
				unsigned int numrows, unsigned int numcols);

	LookupTable(const LookupTable & lt);


	/** @brief Loads a row from file into memory
	 *
	 *  To achieve better performance, one can
	 *	load in whole rows at a time then pick 
	 *	out individual points from the row
	 *
	 *  @param r the row index to be loaded
	 *  @return Void.
	 */
	void load_row(unsigned int r);

	/** @brief Loads a column from file into memory
	 *
	 *  To achieve better performance, one can
	 *	load in whole columns at a time then pick 
	 *	out individual points from the column
	 *
	 *  @param c the column index to be loaded
	 *  @return Void.
	 */
	void load_column(unsigned int c);

	/** @brief Loads a point from the file
	 *
	 *  Load a single data point directly 
	 *	from the lookup table in the file.
	 *	Slow if using this many times in a 
	 *	row
	 *
	 *  @param r the row index to be loaded
	 *	@param c the column index to be loaded
	 *  @return the data point.
	 */
	double get_data(unsigned int r, unsigned int c);

	/** @brief grab a data point from the loaded data
	 *
	 *  If data has been loaded using load_row() or
	 *	load_column(), this function will grab data
	 *	from that loaded row or column
	 *
	 *  @param ind the index to be grabbed
	 *  @return the data point.
	 */
	double get_loaded_data(unsigned int ind) const;


private:
	std::vector<double> m_loaded_data;
	unsigned int m_rows, m_cols;
	unsigned int m_rowoff, m_coloff;
	std::string m_filename;
	StorageMode m_smode;
	LookupMode m_lmode;
	std::ifstream m_file;
	unsigned int m_cur_row;

	void open_file(std::string filename, LookupMode lmode, StorageMode smode,
				   unsigned int rowstart, unsigned int colstart,
				   unsigned int numrows, unsigned int numcols);

	void goto_row(unsigned int r);
	void goto_col(unsigned int c);

	//void goto_start();
};

#endif