#ifndef PARTITION_HPP  
#define PARTITION_HPP

#include "graph.hpp"

#include <iostream>
#include <vector>

using namespace std;

typedef vector<int> ComponentsMap; // Size=N, value returns the component_id
pair<ComponentsMap,int> loadComponents(RoadNetwork *rN, string filename);

#endif