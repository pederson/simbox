#include "../include/MultiSetContainer.hpp"

#include <iostream>
#include <vector>



struct DataItem{
private:
	float mData;

public:	
	float & data() {return mData;};
};


struct Object : public DataItem{
private:
	double mx,my;

public:
	Object(){};
	Object(double px, double py) :mx(px), my(py) {};
	Object(const Object & n) :mx(n.mx), my(n.my) {};

	double & x() {return mx;};
	double & y() {return my;};
};

typedef std::unordered_map<int, Object> base_map_type;
typedef std::vector<Object> base_vector_type;


typedef simbox::set_map<int, Object, std::string> SetMap;
typedef simbox::set_vector<Object, std::string> SetVector;

int main(int argc, char * argv[]){

	std::cout << "///////// SET MAP CONTAINER ////////" << std::endl;
	// do this for a mapped type
	SetMap m;
	m[1] = Object(0,0);
	m[2] = Object(0,1);
	m[3] = Object(1,0);
	m[4] = Object(1,1);
	m[5] = Object(3,3);
	m[6] = Object(3,6);
	m[7] = Object(6,3);
	m[8] = Object(6,6);

	m.add_to_set(m.begin(), "ends");
	m.add_to_set(--m.end(), "ends");

	for(auto it=m.begin(); it!=m.end(); it++){
		std::cout << "key: " << it->first << " x: " << it->second.x() << " y: " << it->second.y() << std::endl;
	}

	for(auto it=m.begin(); it!=m.end(); it++){
		if (it->first % 2 == 0) 	m.add_to_set(it, "even");
		else 						m.add_to_set(it, "odd");
	}
	m.add_to_set(m.find(1), "even");
	m.remove_from_set(m.find(1), "even");

	for (auto it=m.set("even").begin(); it!=m.set("even").end(); it++){
		it->second.x()++;
		it->second.y()++;
	}

	std::cout << "evens: " << std::endl;
	for (auto it=m.set("even").begin(); it!=m.set("even").end(); it++){
		std::cout << "key: " << it->first << " x: " << it->second.x() << " y: " << it->second.y() << std::endl;
	}

	std::cout << "odds: " << std::endl;
	for (auto it=m.set("odd").begin(); it!=m.set("odd").end(); it++){
		std::cout << "key: " << it->first << " x: " << it->second.x() << " y: " << it->second.y() << std::endl;
	}

	std::cout << "ends: " << std::endl;
	for (auto it=m.set("ends").begin(); it!=m.set("ends").end(); it++){
		std::cout << "key: " << it->first << " x: " << it->second.x() << " y: " << it->second.y() << std::endl;
	}


	std::cout << "///////// SET VECTOR CONTAINER ////////" << std::endl;
	// now for a vector container type
	SetVector v;
	v.push_back(Object(0,0));
	v.push_back(Object(0,1));
	v.push_back(Object(1,0));
	v.push_back(Object(1,1));

	for(auto it=v.begin(); it!=v.end(); it++){
		std::cout << "key: " << it-v.begin() << " x: " << it->x() << " y: " << it->y() << std::endl;
	}

	for(auto it=v.begin(); it!=v.end(); it++){
		if ((it - v.begin()) % 2 == 0) 	v.add_to_set(it, "even");
		else 							v.add_to_set(it, "odd");
	}

	std::cout << "evens: " << std::endl;
	for (auto it=v.set("even").begin(); it!=v.set("even").end(); it++){
		std::cout << " x: " << it->x() << " y: " << it->y() << std::endl;
	}

	std::cout << "odds: " << std::endl;
	for (auto it=v.set("odd").begin(); it!=v.set("odd").end(); it++){
		std::cout << " x: " << it->x() << " y: " << it->y() << std::endl;
	}

	return 0;
}