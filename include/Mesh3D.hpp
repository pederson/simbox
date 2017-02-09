/** @file Mesh3D.hpp
 *  @brief file with 3D Mesh class
 *
 *  This contains the base Mesh3D class
 *  and associated functions, defs, and enums
 *
 *  @author D. Pederson
 *  @bug No known bugs.
 */

#ifndef _MESH3D_H
#define _MESH3D_H

#include "Mesh.hpp"

namespace simbox{




/** @class Mesh3D
 *  @brief a mesh in 3 dimensions
 *
 *  a bunch of points connected by edges. Or
 *  from a different perspective, a bunch of 
 *  elements (cells) with shared edges
 *
 */

class Mesh3D : public Mesh<3>{
public:
  // constructor
  Mesh3D()
  : Mesh<3>() {};

  // copy constructor
  // Mesh3D(const Mesh3D & mesh);
  
  // grid generation and refinement
  //static Mesh create_unstructured_tri_simple();

  // reading and writing files
  static std::shared_ptr<Mesh3D> read_MSH(std::string filename);
  // static Mesh read_XML(std::string filename);
  // static Mesh read_NEU(std::string filename, unsigned int byte_offset=0);
  // static Mesh read_CAS(std::string filename, unsigned int byte_offset=0);
  // void write_MSH(std::string filename) const;
  // void write_NEU(std::string filename) const;
  // void write_CAS(std::string filename) const;

protected:


  void read_MSH_internal(std::string filename){
    // open a file stream
    std::ifstream file;
    file.open(filename, std::ifstream::in);
    if (!file.is_open()){
      std::cerr << "Mesh3D: could not open the MSH file!" << std::endl;
      throw -1;
    }

    // msh file info
    float             msh_version;
    unsigned int      msh_filetype, msh_datasize;
    unsigned int      uint_in;
    double            doub_in;
    std::string       st_in;

    // scan to the first line with a $ sign that indicates the MeshFormat section
    std::string line;
    char first;

    getline(file, line);

    while (!file.eof()){
      first = line[0];
      if (first == '$'){
        line.erase(line.begin());
        if (line.compare("MeshFormat")==0){
          // read the version
          file >> msh_version;

          // read the filetype
          file >> msh_filetype;

          // read the datasize
          file >> msh_datasize;

        }
        else{
          std::cerr << "Mesh3D: first section must contain MeshFormat" << std::endl;
          throw -1;
        }
        break;
      }
      getline(file, line);
    }

    if (msh_filetype != 0){
      std::cerr << "Mesh3D: detected binary MSH format. Can only read ASCII" << std::endl;
      throw -1;
    }


    // now parse the rest of the file
    while (!file.eof()){
      getline(file, line);

      if (line[0] == '$'){
        line.erase(line.begin());

        if (line.compare("Nodes")==0){
          //getline(file, line);
          // std::cout << "line: " << line << std::endl;
          file >> uint_in ;
          m_snodes.resize(uint_in);
          // set all the node values accordingly
          for (auto i=0; i<m_snodes.size(); i++){
            getline(file, line);
            file >> uint_in;  // the node index
            // Node<3> nd;
            for (auto j=0; j<3; j++){
              file >> doub_in;  // x coord
              // nd.x[j] = doub_in;
              m_snodes[i].x[j] = doub_in;
            }
            // m_snodes.push_back(nd);
          }

        }
        else if (line.compare("Elements")==0){

          unsigned int elemtype, numtags, numelems;

          //getline(file, line);
          file >> uint_in ;
          //std::cout << "elementcount: " << uint_in << std::endl;
          // set_elementcount(uint_in);
          m_selements.resize(uint_in);
          // set all the node values accordingly
          for (auto i=0; i<m_selements.size(); i++){
            getline(file, line);
            file >> uint_in;  // the element index
            file >> elemtype;  // element type

            switch (elemtype){
              case 1:   // 2 node line
                numelems = 2;
                m_selements[i].type = ElementType::LINE_2;
                break;
              case 2:   // 3 node triangle
                numelems = 3;
                m_selements[i].type = ElementType::TRI_3;
                break;
              case 3:   // 4 node quadrangle
                numelems = 4;
                m_selements[i].type = ElementType::QUAD_4;
                break;
              case 4:   // 4 node tetrahedron
                numelems = 4;
                m_selements[i].type = ElementType::TET_4;
                break;
              case 5:   // 8 node hexahedron
                numelems = 8;
                m_selements[i].type = ElementType::HEX_8;
                break;
              case 6:   // 6 node prism
                numelems = 6;
                m_selements[i].type = ElementType::PRISM_6;
                break;
              case 7:   // 5 node pyramid
                numelems = 5;
                m_selements[i].type = ElementType::PYRAMID_5;
                break;
              // case 8:   // 3 node second order line
              //   numelems = 2;
              //   m_selements[i].type = ElementType::LINE;
              //   break;
              // case 9:   // 6 node second order triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 10:   // 9 node second order quadrangle
              //   numelems = 4;
              //   m_selements[i].type = ElementType::QUADRANGLE;
              //   break;
              // case 11:   // 10 node second order tetrahedron
              //   numelems = 4;
              //   m_selements[i].type = ElementType::TETRAHEDRON;
              //   break;
              // case 12:   // 27 node second order hexahedron
              //   numelems = 8;
              //   m_selements[i].type = ElementType::HEXAHEDRON;
              //   break;
              // case 13:   // 18 node second order prism
              //   numelems = 6;
              //   m_selements[i].type = ElementType::PRISM;
              //   break;
              // case 14:   // 14 node second order pyramid
              //   numelems = 5;
              //   m_selements[i].type = ElementType::PYRAMID;
              //   break;
              case 15:   // 1 node point
                numelems = 1;
                m_selements[i].type = ElementType::POINT_1;
                break;
              // case 20:   // 9 node third order incomplete triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 21:   // 10 node third order incomplete triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 22:   // 12 node fourth order incomplete triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 23:   // 15 node fourth order triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 24:   // 15 node fifth order incomplete triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 25:   // 21 node fifth order complete triangle
              //   numelems = 3;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 26:   // 4 node third order edge
              //   numelems = 2;
              //   m_selements[i].type = ElementType::LINE;
              //   break;
              // case 27:   // 5 node fourth order edge
              //   numelems = 2;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 28:   // 6 node fifth order edge
              //   numelems = 2;
              //   m_selements[i].type = ElementType::TRIANGLE;
              //   break;
              // case 29:   // 20 node third order tetrahedron
              //   numelems = 4;
              //   m_selements[i].type = ElementType::TETRAHEDRON;
              //   break;
              // case 30:   // 35 node fourth order tetrahedron
              //   numelems = 4;
              //   m_selements[i].type = ElementType::TETRAHEDRON;
              //   break;
              // case 31:   // 56 node fifth order tetrahedron
              //   numelems = 4;
              //   m_selements[i].type = ElementType::TETRAHEDRON;
              //   break;
              default:
                std::cout << "ERROR: MSH files with higher order elements are not yet supported!" << std::endl;
                throw -1;
            }

            file >> numtags;
            for (auto j=0; j<numtags; j++) file >> uint_in;

            for (auto j=0; j<numelems; j++){
              file >> uint_in; // read node index associated with the element
              m_selements[i].nodeinds.push_back(uint_in-1); // minus 1 because indexing starts at 0
            }
            
          }

        }
        else if (line.compare("NodeData")==0){
          std::cout << "WARNING: MSH NodeData will be lost. Cannot yet read" << std::endl;
        }
        else if (line.compare("ElementData")==0){
          std::cout << "WARNING: MSH ElementData will be lost. Cannot yet read" << std::endl;
        }
        else if (line.substr(0, 3).compare("End")==0){
          continue;
        }
        else{
          std::cout << "Data from section " << line << " will be lost" << std::endl;
        }

      }
    }

    calc_extents();
    return;
  }


};


std::shared_ptr<Mesh3D> Mesh3D::read_MSH(std::string filename){
  std::shared_ptr<Mesh3D> outmesh(new Mesh3D());
  outmesh->read_MSH_internal(filename);
  return outmesh;
}



}
#endif
