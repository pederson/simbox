#include <iostream>

#include "include/Mesh.hpp"
#include "include/Mesh3D.hpp"
#include "include/Mesh2D.hpp"

#include "include/SignalGenerator.hpp"
#include "include/SignalProcessing.hpp"
#include "include/LookupTable.hpp"
#include "include/Timer.hpp"

#include <mpi.h>
#include "include/mpitools.hpp"

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


  cout << "testing 2D mesh" << endl;
  auto mesh2 = simbox::Mesh2D::read_MSH("data/channel.msh");
  mesh2->print_summary();
  cout << "succeeded" << endl;

  cout << "attempting to initialize MPI..." << endl;
  mpi::init(&argc, &argv);
  cout << "done" << endl;
  
  return 0;
}