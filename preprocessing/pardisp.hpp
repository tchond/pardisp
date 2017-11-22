#ifndef PARDISP_HPP  
#define PARDISP_HPP

#include "algos.hpp"
#include "../graph/partition.hpp"
#include "../ch/SubgraphCH.hpp"

using namespace std;

/*
	Definitions
*/

typedef vector<bool> BordersMap; // Size=N, value returns whether the node is a border or not (we use to such maps for incoming and outgoing)
typedef vector<vector<int>> BordersStore; // Size=|components|, size of each set varies. Each set contains the id's of the borders ordered by value
typedef vector<vector<bool>> CompExtensionsMap; // Size=N, size of each vector=|components|, each value shows whether the node is in a given extended component.

typedef vector<vector<int>> DistanceTable; // Size=N, size of each vector=[node.component.borders.size], these values are the distances from one node to all the borders of the same component
typedef vector<vector<vector<int>>> ComponentDistanceMatrix; // Size=|components|x|components|x(|B_inc(C) x B_out(C)|

/*
	Classes for modeling the algorithm
*/

struct ParDiSP {
	RoadNetwork *rN;
	ComponentsMap cm;
	CompExtensionsMap cem;
	ComponentDistanceMatrix cdm;
	int numComponents;
	
	// Structures required by ParDiSP
	BordersMap incBordersMap; BordersMap outBordersMap;
	BordersStore incBordersStore; BordersStore outBordersStore;
	DistanceTable incIDT; DistanceTable outIDT; // Distances to/from each node from/to each outgoing border node 
	
	SubgraphCH *transitNet;

	set<pair<int,int>> computeIDTOutTransitPart(int component, unordered_set<int> &sources, vector<int> &targets);
	void computeIDTInc(int component, unordered_set<int> &sources, vector<int> &targets);
	void computeCDM(vector<bool> &transitGraphNodes);
	void computeCDMWithCH(vector<NodeID> &transitNodes);
	
	int distance_local(int source, int target);
	Path shortest_path_local(int source, int target);

	ParDiSP();
	//~ParDiSP();
	ParDiSP(RoadNetwork *rN, ComponentsMap &cm, int components);
	void preprocessing();
	int distance(int source, int target);
	Path shortest_path(int source, int target);
	
	// More functions for exporting the pre-computed structures
	void precomputeExtendedComponents();
	void preprocessingWithExtComp();
	
	void computeBorders();
	void loadExtendedComponents(string filename);
};

#endif