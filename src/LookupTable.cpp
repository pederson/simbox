#include "LookupTable.hpp"

using namespace std;

LookupTable::LookupTable(){
}


LookupTable::LookupTable(std::string filename, LookupMode lmode, StorageMode smode,
			unsigned int rowstart, unsigned int colstart,
			unsigned int numrows, unsigned int numcols)
: m_rows(numrows)
, m_cols(numcols)
, m_rowoff(rowstart)
, m_coloff(colstart)
, m_filename(filename)
, m_lmode(lmode)
, m_smode(smode)
, m_cur_row(0){
	// open the file
	open_file(filename, m_lmode, m_smode,
			  m_rowoff, m_coloff, m_rows, m_cols);
	// allocate stuff for the internal data
	m_loaded_data.resize(m_cols);
}

LookupTable::LookupTable(const LookupTable & lt)
: m_rows(lt.m_rows)
, m_cols(lt.m_cols)
, m_rowoff(lt.m_rowoff)
, m_coloff(lt.m_coloff)
, m_filename(lt.m_filename)
, m_lmode(lt.m_lmode)
, m_smode(lt.m_smode)
, m_loaded_data(lt.m_loaded_data)
, m_cur_row(lt.m_cur_row)
{
	open_file(m_filename, m_lmode, m_smode,
			  m_rowoff, m_coloff, m_rows, m_cols);
}


void LookupTable::load_row(unsigned int r){
	if (m_cur_row == r) return;
	goto_row(m_rowoff+r);
	//goto_col(m_coloff);
	string line;
	double dummy;
	getline(m_file, line); 
	stringstream ss(line);
	for (auto j=0; j<m_coloff; j++){
			ss >> dummy;
	}
	unsigned int i=0; 
	while (ss >> m_loaded_data[i]) i++;
	m_cur_row = r;
}


void LookupTable::load_column(unsigned int c){
	goto_row(m_rowoff);
	string line;
	double dummy;
	for (auto i=0; i<m_rows; i++){
		getline(m_file, line);
		stringstream ss(line);
		for (auto j=0; j<m_coloff+c; j++){
			ss >> dummy;
		}
		ss >> m_loaded_data[i];
	}
}


double LookupTable::get_data(unsigned int r, unsigned int c){
	double pt;
	goto_row(r);
	string line;
	getline(m_file, line);
	double dummy;
	stringstream ss(line);
	for (auto j=0; j<m_coloff+c; j++){
		ss >> dummy;
	}
	ss >> pt;
	return pt;
}


// grab a data point from the already loaded data
double LookupTable::get_loaded_data(unsigned int ind) const{
	return m_loaded_data[ind];
}

void LookupTable::open_file(std::string filename, LookupMode lmode, StorageMode smode,
			   unsigned int rowstart, unsigned int colstart,
			   unsigned int numrows, unsigned int numcols){
	
	if (lmode == LookupMode::ASCII){
		m_file.open(m_filename, std::ifstream::in);
	}
	else{
		cout << "BINARY LOOKUP TABLE ISNT YET SUPPORTED" << endl;
		throw -1;
	}
}

void LookupTable::goto_row(unsigned int r){
	string line;
	m_file.seekg(0, ios_base::beg);
	for (auto i=0; i<r; i++){
		getline(m_file, line);
	}
}

void LookupTable::goto_col(unsigned int c){
	string line;
	// get current line
	stringstream ss(line);
	double dummy;
	for (auto j=0; j<m_coloff; j++){
		ss >> dummy;
	}
}

