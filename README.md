# simbox
Toolbox for numerical discretizations of PDEs
* General Mesh class for a mesh of arbitrary dimension
* Mesh3D, Mesh2D, and Mesh1D for general 1, 2, and 3D mesh
	* enables reading of standard mesh file formats (MSH, NEU, XML, CAS)
* Domain class to hold parallelization information
* SimulationData base class for data output from a general Mesh
	* implementation by HDF5, VTK
* utilities
	* Timer
	* LookupTable
	* SignalGenerator
	* SignalProcessing