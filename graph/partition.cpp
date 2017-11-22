#include "partition.hpp"

pair<ComponentsMap,int> loadComponents(RoadNetwork *rN, string filename) {
	pair<ComponentsMap,int> map;
	map.first.reserve((rN->numNodes));
	
	ifstream graphIn(filename.c_str());
	
	if (!graphIn.is_open()) { 
	 	cerr << "Cannot open " << filename << endl; 
	   	exit(EXIT_FAILURE);
	}
	
	string line;
	int verticesNum = -1, edgesNum = -1;
	int max = -1;
	
	for(int i=0;i<rN->numNodes;i++) {
		getline(graphIn,line);
		
		int part;
		istringstream(line) >> part;
		map.first.push_back(part);
		if(part > max)
			max = part;
	}
	map.second = max+1;
	
	graphIn.close();
	return map;
}