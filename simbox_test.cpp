#include <iostream>

#include "include/Mesh.hpp"
#include "include/Mesh3D.hpp"

using namespace std;

int main(int argc, char * argv[]){

  // test constructor
  cout << "testing mesh constructor..." << std::endl;
  simbox::Mesh<2> * mymesh = new simbox::Mesh<2>();
  mymesh->print_summary();
  cout << "succeeded" << endl;


  cout << "testing 3D mesh" << endl;
  auto mesh3 = simbox::Mesh3D::read_MSH("data/channel.msh");
  mesh3->print_summary();
  cout << "succeeded" << endl;

  return 0;
}