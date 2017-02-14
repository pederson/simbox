#include <iostream>

#include "include/Mesh.hpp"
#include "include/Mesh3D.hpp"
#include "include/Mesh2D.hpp"
#include "include/RegularMesh3D.hpp"
#include "include/RegularMesh2D.hpp"

#include "include/SignalGenerator.hpp"
#include "include/SignalProcessing.hpp"
#include "include/LookupTable.hpp"
#include "include/Timer.hpp"

#include <mpi.h>
#include "include/mpitools.hpp"

#include "include/Domain.hpp"
#include "include/SimulationData.hpp"
#include "include/SimulationDataHDF.hpp"


class DomainOf1 : public simbox::Domain<3>{
public:
	unsigned int nodecount(int proc) const {return 4325;};
	unsigned int elementcount(int proc) const {return 8656;};
	unsigned int nodecount() const {return 4325;};
	unsigned int elementcount() const {return 8656;};
	unsigned int numprocs() const {return mpi::size();};
};

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

  cout << "testing 3D regularmesh" << endl;
  auto rmesh3 = simbox::RegularMesh3D::generate({20,20,20},{0.05,0.05,0.05},{0,0,0});
  rmesh3->print_summary();
  cout << "succeeded" << endl;


  cout << "testing 2D mesh" << endl;
  auto mesh2 = simbox::Mesh2D::read_MSH("data/channel.msh");
  mesh2->print_summary();
  cout << "succeeded" << endl;

  cout << "testing 2D regularmesh" << endl;
  auto rmesh2 = simbox::RegularMesh2D::generate({20,20},{0.05,0.05},{0,0});
  rmesh2->print_summary();
  cout << "succeeded" << endl;

  cout << "attempting to initialize MPI..." << endl;
  mpi::init(&argc, &argv);
  cout << "done" << endl;

  DomainOf1 dom3;
  simbox::SimulationDataHDF<3> sdH5("H5Data.h5", dom3, mesh3, {0.0,1.0});
  sdH5.add_nodefield("Doodee");
  sdH5.add_elemfield("Caca");
  sdH5.print_summary();
  sdH5.write_XDMF();

  cout << "finito" << endl;

  return 0;
}