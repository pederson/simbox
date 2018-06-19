#include "../include/GenericMesh.hpp"

#include <iostream>
#include <vector>



struct DataItem{
private:
	float mData;

public:	
	float & data() {return mData;};
};


struct NodeObject : public DataItem{
private:
	double mx,my;

public:
	NodeObject(double px, double py) :mx(px), my(py) {};
	NodeObject(const NodeObject & n) :mx(n.mx), my(n.my) {};

	double & x() {return mx;};
	double & y() {return my;};
};





typedef std::vector<NodeObject> 			node_base;
typedef std::vector<std::pair<int, int>> 	cell_base;
 

struct node_cont : public node_base{
	node_cont & nodes() {return *this;};
};

struct edge_cont : public node_base{
	edge_cont & edges() {return *this;};
};

struct cell_cont : public cell_base{
	cell_cont & cells() {return *this;};
};


int main(int argc, char * argv[]){
	simbox::GenericMesh<node_cont, void, cell_cont> mesh;

	std::cout << "cell size: " << mesh.cells().size()  << std::endl;


	// push in 10 nodes from 0.0 to 1.0
	for (double x=0.0; x<1.01; x+=0.05) mesh.nodes().push_back(NodeObject(x,0));

	// connect connect the nodes to make cells
	for (auto i=0; i<mesh.nodes().size()-1; i++){
		mesh.cells().push_back(std::make_pair(i,i+1));
	}

	// connect connect the nodes to make cells
	for (auto i=0; i<mesh.nodes().size(); i++){
		mesh.nodes().at(i).data() = i;
		// mesh.cells().push_back(std::make_pair(i,i+1));
	}

	std::cout << "num nodes: " << mesh.nodes().size() << std::endl;
	std::cout << "num cells: " << mesh.cells().size() << std::endl;

	// print out the stuff
	for (auto it=mesh.nodes().begin(); it!=mesh.nodes().end(); it++){
		std::cout << "x: " << (*it).x() << " \tdata: " << (*it).data() << std::endl;
	}

	// // print out the stuff
	// for (auto it=mesh.cells().begin(); it!=mesh.cells().end(); it++){
	// 	std::cout << "x: " << (*it).x() << std::endl;
	// }

	return 0;
}