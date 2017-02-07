/************************************************************************************//**
 * \file function_approx.cpp
 * 
 * File filled with necessary code for function approximation
 *
 ***************************************************************************************/

#include "Mesh.hpp"

//#define _TEST_

using namespace std;

MeshNode::MeshNode(){
  _x = 0.0;
  _y = 0.0;
  _z = 0.0;
  _boundary = false;
  _core_group = 0;
  _num_connections = 0;
}

MeshNode::MeshNode(double x, double y, double z, bool boundary, unsigned int num_connections, unsigned int core_group){
  _x = x;
  _y = y;
  _z = z;
  _boundary = boundary;
  _num_connections = num_connections;
  _core_group = core_group;
}

MeshNode::~MeshNode(){

}

void MeshNode::print_summary(ostream & os) const {
  os << "  MeshNode: (" << _x << ", " << _y << ", " << _z 
        << ")    boundary?: " << (_boundary? "yes" : "no") 
        << "   num_connections: " << _num_connections
        << "   core_group: " << _core_group << endl;
}

void MeshNode::print_detailed(ostream & os) const{
  os << "  MeshNode: (" << _x << ", " << _y << ", " << _z 
        << ")    boundary?: " << (_boundary? "yes" : "no") 
        << "   num_connections: " << _num_connections
        << "   core_group: " << _core_group << endl;
}

double MeshNode::dist_sq(const MeshNode & node1, const MeshNode & node2){
  return (node1.x()-node2.x())*(node1.x()-node2.x()) + 
          (node1.y()-node2.y())*(node1.y()-node2.y()) + 
          (node1.z()-node2.z())*(node1.z()-node2.z());
}

double MeshNode::area_tri(const MeshNode & node1, const MeshNode & node2, const MeshNode & node3){
  return fabs((node2.x()*node1.y() - node1.x()*node2.y()) + 
              (node3.x()*node2.y() - node2.x()*node3.y()) + 
              (node1.x()*node3.y() - node3.x()*node1.y()));
}
//******************************************************************************************************

MeshElement::MeshElement(){
  _element_type = ElementType::EMPTY_0;
}

MeshElement::MeshElement(std::vector<unsigned int> vertex_inds){
  _vertex_inds = vertex_inds;
  recalc_type();
}

MeshElement::~MeshElement(){

}

void MeshElement::print_summary(ostream & os) const{
  //if (mpi::my_proc() != mpi::master_proc()) return;
  os << "  MeshElement: num_elements: " << _vertex_inds.size() << " vertices: ";
  os << "  \t Topology: " << get_string(_element_type) << endl;
  for (unsigned int i=0; i<_vertex_inds.size(); i++){
    os << _vertex_inds.at(i) << "  ";
  }
  os << endl;
  return;
}

void MeshElement::print_detailed(ostream & os) const{
  //if (mpi::my_proc() != mpi::master_proc()) return;
  os << " Mesh Element: " << _vertex_inds.size() << " vertices: ";
  for (unsigned int i=0; i<_vertex_inds.size(); i++){
    os << _vertex_inds.at(i) << ", ";
  }
  os << endl;
  return;
}


void MeshElement::remove_vertex(unsigned int vert_ind){
  for (unsigned int i=0; i<_vertex_inds.size(); i++){
    if (_vertex_inds.at(i) == vert_ind){
      // remove index
      _vertex_inds.erase(_vertex_inds.begin()+i);
      break;
    }
  }
  recalc_type();
}

void MeshElement::add_vertex(unsigned int vert_ind, int position){
  if (position == -1){
    _vertex_inds.push_back(vert_ind);
  }
  else{
    _vertex_inds.insert(_vertex_inds.begin() + position, vert_ind);
  }
  //recalc_type();
}
 
void MeshElement::recalc_type(){
  cout << "doing a recalc doesn't have a defined behavior yet!" << endl;
  switch (_vertex_inds.size()){
    case 0:
      _element_type = ElementType::EMPTY_0;
      break;

    case 1:
      _element_type = ElementType::POINT_1;
      break;

    case 2:
      _element_type = ElementType::LINE_2;
      break;

    case 3:
      _element_type = ElementType::TRI_3;
      break;

    case 4:
      _element_type = ElementType::QUAD_4;
      break;

    case 8:
      _element_type = ElementType::HEX_8;
      break;

    case 5:
      _element_type = ElementType::UNKNOWN;
      break;

    otherwise:
      _element_type = ElementType::UNKNOWN;
  }
}

//******************************************************************************************************
Mesh::Mesh(){
  _xmin = 0;
  _xmax = 0;
  _ymin = 0;
  _ymax = 0;
  _zmin = 0;
  _zmax = 0;
  _mesh_type = MESH_UNKNOWN;
  _num_dims = 0;
}

Mesh::~Mesh(){

}

void Mesh::print_summary(ostream & os) const{
  //if (mpi::my_proc() != mpi::master_proc()) return;
  os << " " << endl;
  os << "******** Mesh Summary ******** " << endl;
  if (_nodes.size() == 0){
    os << "  Mesh is empty!" << endl;
    return;
  }

  os << "  type: ";
  if (_mesh_type==REGULAR) os << "REGULAR" << endl;
  else if (_mesh_type==UNSTRUCTURED_TRI) os << "UNSTRUCTURED TRI" << endl;
  else if (_mesh_type==UNSTRUCTURED_QUAD) os << "UNSTRUCTURED QUAD" << endl;
  else os << "MESH_UNKNOWN" << endl;
  os << "  num_dims: " << _num_dims << endl;
  os << "  num_nodes: " << _nodes.size() << endl;
  os << "  num_elements: " << _elements.size() << endl;
  os << "  x extents: [" << _xmin << ", " << _xmax << "]" << endl;
  os << "  y extents: [" << _ymin << ", " << _ymax << "]" << endl;
  os << "  z extents: [" << _zmin << ", " << _zmax << "]" << endl;
  os << "  NodeData (" << m_nodedata.size() << "): " << endl;
  for (auto i=m_nodedata.begin(); i!=m_nodedata.end(); i++) os << "      " << i->first << endl;
  os << "  ElementData (" << m_elementdata.size() << "): " << endl;
  for (auto i=m_elementdata.begin(); i!=m_elementdata.end(); i++) os << "      " << i->first << endl;

  if (_nodes.size() > 0) {
    os << "  last node: ";
    _nodes.back().print_summary(os);
  }
  if (_elements.size() > 0){
    os << "  last element: ";
    _elements.back().print_summary(os);
  }
  os << "************************************* " << endl;
  os << " " << endl;

  return;
}

void Mesh::print_detailed(ostream & os) const{

}

unsigned int Mesh::nearest_node(double x_loc, double y_loc, double z_loc) const{
  double * dsq = new double[_nodes.size()];
  for (auto i=0; i<_nodes.size(); i++){
    dsq[i] = (x_loc - _nodes[i].x())*(x_loc - _nodes[i].x())
            +(y_loc - _nodes[i].y())*(y_loc - _nodes[i].y())
            +(z_loc - _nodes[i].z())*(z_loc - _nodes[i].z());
  }

  unsigned int minloc = 0;
  double minval = dsq[0];
  for (auto i=1; i<_nodes.size(); i++){
    if (dsq[i] < minval){
      minloc = i;
      minval = dsq[i];
    }  
  }
  delete[] dsq;

  return minloc;
}


double Mesh::get_element_center_x(unsigned int i) const{
  // get the vertices for this element
  const MeshElement & el = element(i);
  unsigned int nverts = el.num_vertices();
  double cent = 0.0;
  for (auto j=0; j<nverts; j++){
    cent += node(el.vertex_ind(j)).x();
  }
  cent /= double(nverts);
  return cent;
}

double Mesh::get_element_center_y(unsigned int i) const{
  // get the vertices for this element
  const MeshElement & el = element(i);
  unsigned int nverts = el.num_vertices();
  double cent = 0.0;
  for (auto j=0; j<nverts; j++){
    cent += node(el.vertex_ind(j)).y();
  }
  cent /= double(nverts);
  return cent;
}

double Mesh::get_element_center_z(unsigned int i) const{
  // get the vertices for this element
  const MeshElement & el = element(i);
  unsigned int nverts = el.num_vertices();
  double cent = 0.0;
  for (auto j=0; j<nverts; j++){
    cent += node(el.vertex_ind(j)).z();
  }
  cent /= double(nverts);
  return cent;
}



const double & Mesh::x() {
  if (_x.size() != _nodes.size()){
    _x.clear();
    _x.resize(_nodes.size());
    for (unsigned int i=0; i<_nodes.size(); i++){
      _x.at(i) = _nodes.at(i).x();
    }
  }
  return _x.front();
}


const double & Mesh::y(){
  if (_y.size() != _nodes.size()){
    _y.clear();
    _y.resize(_nodes.size());
    for (unsigned int i=0; i<_nodes.size(); i++){
      _y.at(i) = _nodes.at(i).y();
    }
  }
  return _y.front();
}

const double & Mesh::z(){
  if (_z.size() != _nodes.size()){
    _z.clear();
    _z.resize(_nodes.size());
    for (unsigned int i=0; i<_nodes.size(); i++){
      _z.at(i) = _nodes.at(i).z();
    }
  }
  return _z.front();
}


const bool & Mesh::boundary(){
  if (_boundary.size() != _nodes.size()){
    _boundary.clear();
    _boundary.resize(_nodes.size());
    for (unsigned int i=0; i<_nodes.size(); i++){
      _boundary.at(i) = _nodes.at(i).boundary();
    }
  }
  return _boundary.front();
}


const unsigned int & Mesh::core_group(){
  if (_core_group.size() != _nodes.size()){
    _core_group.clear();
    _core_group.resize(_nodes.size());
    for (unsigned int i=0; i<_nodes.size(); i++){
      _core_group.at(i) = _nodes.at(i).core_group();
    }
  }
  return _core_group.front();
}

const unsigned int & Mesh::num_connections(){
  if (_num_connections.size() != _nodes.size()){
    _num_connections.clear();
    _num_connections.resize(_nodes.size());
    for (unsigned int i=0; i<_nodes.size(); i++){
      _num_connections.at(i) = _nodes.at(i).num_connections();
    }
  }
  return _num_connections.front();
}

const double & Mesh::data(std::string fieldname) const{

  if (_phys_properties.at(fieldname).size() != _nodes.size()){
    cout << "physical properties size doesn't match nodes size!" << endl;
    throw -1;
  }

  return _phys_properties.at(fieldname).front();
}

const double * Mesh::nodedata(std::string fieldname) const{

  return &(m_nodedata.at(fieldname).front());
}

const double * Mesh::elementdata(std::string fieldname) const{

  return &(m_elementdata.at(fieldname).front());
}

void Mesh::set_nodecount(unsigned int count){
  if (_nodes.size() > 0){
    cout << "cannot expand mesh if it is already allocated!" << endl;
    throw -1;
  }

  _nodes.resize(count);

}

void Mesh::set_elementcount(unsigned int count){
  if (_elements.size() > 0){
    cout << "cannot expand mesh if it is already allocated!" << endl;
    throw -1;
  }

  _elements.resize(count);

}

void Mesh::add_phys_property(std::string property_name, const double * property_vals){
  for (unsigned int i=0; i<_phys_property_names.size(); i++){
    if (_phys_property_names.at(i).compare(property_name) == 0){
      cout << property_name << " is already in the mesh... doing nothing" << endl;
      return;
    }
  }

  vector<double> prop;
  //const double * valptr = &property_vals;
  prop.assign(_nodes.size(), 0.0);
  for (unsigned int i=0; i<_nodes.size(); i++) prop.at(i) = property_vals[i];
  _phys_property_names.push_back(property_name);
  _phys_properties[property_name] = prop;
  return;
}

void Mesh::add_phys_property(std::string property_name, double init_val){
  for (unsigned int i=0; i<_phys_property_names.size(); i++){
    if (_phys_property_names.at(i).compare(property_name) == 0){
      cout << property_name << " is already in the mesh... doing nothing" << endl;
      return;
    }
  }

  vector<double> prop;
  prop.assign(_nodes.size(), init_val);
  _phys_property_names.push_back(property_name);
  _phys_properties[property_name] = prop;
  return;
}

void Mesh::add_nodedata(std::string property_name, const double * values){
  // for (unsigned int i=0; i<_phys_property_names.size(); i++){
  //   if (_phys_property_names.at(i).compare(property_name) == 0){
  //     break;
  //   }
  //   _phys_property_names.push_back(property_name);
  // }

  vector<double> prop;
  prop.assign(_nodes.size(), 0.0);
  for (unsigned int i=0; i<_nodes.size(); i++) prop.at(i) = values[i];
  m_nodedata[property_name] = prop;
}

void Mesh::add_nodedata(std::string property_name, double init_val){
  // for (unsigned int i=0; i<_phys_property_names.size(); i++){
  //   if (_phys_property_names.at(i).compare(property_name) == 0){
  //     break;
  //   }
  //   _phys_property_names.push_back(property_name);
  // }

  vector<double> prop;
  prop.assign(_nodes.size(), init_val);
  m_nodedata[property_name] = prop;
}

void Mesh::add_elementdata(std::string property_name, const double * values){
  // for (unsigned int i=0; i<_phys_property_names.size(); i++){
  //   if (_phys_property_names.at(i).compare(property_name) == 0){
  //     break;
  //   }
  //   _phys_property_names.push_back(property_name);
  // }


  vector<double> prop;
  prop.assign(_elements.size(), 0.0);
  for (unsigned int i=0; i<_elements.size(); i++) prop.at(i) = values[i];
  m_elementdata[property_name] = prop;
}

void Mesh::add_elementdata(std::string property_name, double init_val){
  // for (unsigned int i=0; i<_phys_property_names.size(); i++){
  //   if (_phys_property_names.at(i).compare(property_name) == 0){
  //     break;
  //   }
  //   _phys_property_names.push_back(property_name);
  // }

  vector<double> prop;
  prop.assign(_elements.size(), init_val);
  m_elementdata[property_name] = prop;
}

std::vector<std::string> Mesh::get_nodedata_names() const{
  vector<string> names;
  for (auto p=m_nodedata.begin(); p!=m_nodedata.end(); p++){
    names.push_back(p->first);
  }
  return names;
}

std::vector<std::string> Mesh::get_elementdata_names() const{
  vector<string> names;
  for (auto p=m_elementdata.begin(); p!=m_elementdata.end(); p++){
    names.push_back(p->first);
  }
  return names;
}


void Mesh::reset_property(std::string property_name, double reset_val){
  for (unsigned int i=0; i<_phys_property_names.size(); i++){
    if (_phys_property_names.at(i).compare(property_name) == 0){
      cout << property_name << " is already in the mesh... doing nothing" << endl;
      return;
    }
  }

  return;
}


void Mesh::calc_extents(){
  // declare vars

  _xmax = _nodes.at(0).x(); _xmin = _nodes.at(0).x(); _ymax = _nodes.at(0).y(); _ymin = _nodes.at(0).y(); _zmax = _nodes.at(0).z(); _zmin = _nodes.at(0).z();
  for (unsigned int i=1; i<_nodes.size(); i++){
    if (_nodes.at(i).x() > _xmax) _xmax = _nodes.at(i).x();
    if (_nodes.at(i).x() < _xmin) _xmin = _nodes.at(i).x();
    if (_nodes.at(i).y() > _ymax) _ymax = _nodes.at(i).y();
    if (_nodes.at(i).y() < _ymin) _ymin = _nodes.at(i).y();
    if (_nodes.at(i).z() > _zmax) _zmax = _nodes.at(i).z();
    if (_nodes.at(i).z() < _zmin) _zmin = _nodes.at(i).z();
  }
  return;
}

Mesh Mesh::read_MSH(string filename){
  Mesh outmesh;
  outmesh.read_MSH_internal(filename);

  return outmesh;
}

void Mesh::read_MSH_internal(string filename){

  // open a file stream
  ifstream file;
  file.open(filename, ifstream::in);
  if (!file.is_open()){
    cout << "ERROR: could not open the MSH file!" << endl;
    throw -1;
  }

  // standard stufff
  _num_dims = 3;

  // msh file info
  float msh_version;
  unsigned int msh_filetype, msh_datasize;
  unsigned int uint_in;
  double doub_in;
  string st_in;

  // scan to the first line with a $ sign that indicates the MeshFormat section
  string line;
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
        cout << "ERROR: first section must contain MeshFormat" << endl;
        throw -1;
      }
      break;
    }
    getline(file, line);
  }

  if (msh_filetype != 0){
    cout << "ERROR: detected binary MSH format. Can only read ASCII" << endl;
    throw -1;
  }


  // now parse the rest of the file
  while (!file.eof()){
    getline(file, line);

    if (line[0] == '$'){
      line.erase(line.begin());

      if (line.compare("Nodes")==0){
        //getline(file, line);
        //cout << "line: " << line << endl;
        file >> uint_in ;
        //cout << "nodecount: " << uint_in << endl;
        set_nodecount(uint_in);

        // set all the node values accordingly
        for (auto i=0; i<_nodes.size(); i++){
          getline(file, line);
          file >> uint_in;  // the node index
          file >> doub_in;  // x coord
          _nodes[i].set_x(doub_in);
          file >> doub_in;  // y coord
          _nodes[i].set_y(doub_in);
          file >> doub_in;  // z coord
          _nodes[i].set_z(doub_in);
        }
        

      }
      else if (line.compare("Elements")==0){

        unsigned int elemtype, numtags, numelems;

        //getline(file, line);
        file >> uint_in ;
        //cout << "elementcount: " << uint_in << endl;
        set_elementcount(uint_in);

        // set all the node values accordingly
        for (auto i=0; i<_elements.size(); i++){
          getline(file, line);
          file >> uint_in;  // the element index
          file >> elemtype;  // element type

          switch (elemtype){
            case 1:   // 2 node line
              numelems = 2;
              _elements[i].set_element_type(ElementType::LINE_2);
              break;
            case 2:   // 3 node triangle
              numelems = 3;
              _elements[i].set_element_type(ElementType::TRI_3);
              break;
            case 3:   // 4 node quadrangle
              numelems = 4;
              _elements[i].set_element_type(ElementType::QUAD_4);
              break;
            case 4:   // 4 node tetrahedron
              numelems = 4;
              _elements[i].set_element_type(ElementType::TET_4);
              break;
            case 5:   // 8 node hexahedron
              numelems = 8;
              _elements[i].set_element_type(ElementType::HEX_8);
              break;
            case 6:   // 6 node prism
              numelems = 6;
              _elements[i].set_element_type(ElementType::PRISM_6);
              break;
            case 7:   // 5 node pyramid
              numelems = 5;
              _elements[i].set_element_type(ElementType::PYRAMID_5);
              break;
            // case 8:   // 3 node second order line
            //   numelems = 2;
            //   _elements[i].set_element_type(ElementType::LINE);
            //   break;
            // case 9:   // 6 node second order triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 10:   // 9 node second order quadrangle
            //   numelems = 4;
            //   _elements[i].set_element_type(ElementType::QUADRANGLE);
            //   break;
            // case 11:   // 10 node second order tetrahedron
            //   numelems = 4;
            //   _elements[i].set_element_type(ElementType::TETRAHEDRON);
            //   break;
            // case 12:   // 27 node second order hexahedron
            //   numelems = 8;
            //   _elements[i].set_element_type(ElementType::HEXAHEDRON);
            //   break;
            // case 13:   // 18 node second order prism
            //   numelems = 6;
            //   _elements[i].set_element_type(ElementType::PRISM);
            //   break;
            // case 14:   // 14 node second order pyramid
            //   numelems = 5;
            //   _elements[i].set_element_type(ElementType::PYRAMID);
            //   break;
            case 15:   // 1 node point
              numelems = 1;
              _elements[i].set_element_type(ElementType::POINT_1);
              break;
            // case 20:   // 9 node third order incomplete triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 21:   // 10 node third order incomplete triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 22:   // 12 node fourth order incomplete triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 23:   // 15 node fourth order triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 24:   // 15 node fifth order incomplete triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 25:   // 21 node fifth order complete triangle
            //   numelems = 3;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 26:   // 4 node third order edge
            //   numelems = 2;
            //   _elements[i].set_element_type(ElementType::LINE);
            //   break;
            // case 27:   // 5 node fourth order edge
            //   numelems = 2;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 28:   // 6 node fifth order edge
            //   numelems = 2;
            //   _elements[i].set_element_type(ElementType::TRIANGLE);
            //   break;
            // case 29:   // 20 node third order tetrahedron
            //   numelems = 4;
            //   _elements[i].set_element_type(ElementType::TETRAHEDRON);
            //   break;
            // case 30:   // 35 node fourth order tetrahedron
            //   numelems = 4;
            //   _elements[i].set_element_type(ElementType::TETRAHEDRON);
            //   break;
            // case 31:   // 56 node fifth order tetrahedron
            //   numelems = 4;
            //   _elements[i].set_element_type(ElementType::TETRAHEDRON);
            //   break;
            default:
              cout << "ERROR: MSH files with higher order elements are not yet supported!" << endl;
              throw -1;
          }

          file >> numtags;
          for (auto j=0; j<numtags; j++) file >> uint_in;

          for (auto j=0; j<numelems; j++){
            file >> uint_in; // read node index associated with the element
            _elements[i].add_vertex(uint_in-1); // minus 1 because indexing starts at 0
          }
          
        }

      }
      else if (line.compare("NodeData")==0){
        cout << "WARNING: MSH NodeData will be lost. Cannot yet read" << endl;
      }
      else if (line.compare("ElementData")==0){
        cout << "WARNING: MSH ElementData will be lost. Cannot yet read" << endl;
      }
      else if (line.substr(0, 3).compare("End")==0){
        continue;
      }
      else{
        cout << "Data from section " << line << " will be lost" << endl;
      }

    }
  }

  calc_extents();
  return;
}






//*************************************************************************************************************

#ifdef _TEST_

// to compile: g++ -std=c++11 Mesh.cpp -o mesh_test

int main(int argc, char * argv[]){

  // test constructor
  cout << "testing mesh constructor..." << flush;
  Mutable_Mesh * mymesh = new Mutable_Mesh();
  //Mesh * mymesh = new Mesh();
  cout << "succeeded" << endl;

  // test num nodes setting
  cout << "testing node setting..." << flush;
  mymesh->set_num_nodes(100);
  cout << "succeeded" << endl;

  // test creation of a regular mesh
  cout << "testing regular grid creation..." << flush;
  Mutable_Mesh * mesh_reg_1d = Mutable_Mesh::create_regular_grid(0.1, 100);
  cout << "succeeded" << endl;
  mesh_reg_1d->print_summary();
  mesh_reg_1d->get_node_ptr(50)->print_summary();

  // test creation of a regular mesh
  cout << "testing regular grid creation..." << flush;
  Mesh * mesh_reg_1ds = Mesh::create_regular_grid_n(0.1, 100, 50, 10);
  cout << "succeeded" << endl;
  mesh_reg_1ds->print_summary();
  mesh_reg_1ds->node(50).print_summary();
  mesh_reg_1ds->element(50).print_summary();

  // test the node addition
  cout << "testing node insertion..." << flush;
  mesh_reg_1d->add_node(new Node);
  cout << "succeeded" << endl;
  mesh_reg_1d->print_summary();

  // test the node deletion
  cout << "testing node deletion..." << flush;
  mesh_reg_1d->remove_node(100);
  cout << "succeeded" << endl;
  mesh_reg_1d->print_summary();


  cout << "testing mesh deletion..." << flush;
  delete mesh_reg_1d;
  cout << "succeeded" << endl;

  return 0;
}

#endif

