/* Copyright (C) 2017
 * Theodoros Chondrogiannis
 * Free University of Bozen-Bolzano
 *
 * This file is part of ParDiSP.
 *
 * ParDiSP is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * ParDiSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with ParDiSP; see the file COPYING; if not,
 * see <http://www.gnu.org/licenses/>.
 */
 
 #ifndef PARDISP_HPP  
#define PARDISP_HPP

#include <unordered_set>
#include <cmath> 

#include "../graph/graph.hpp"
#include "../graph/partition.hpp"
#include "../ch/SubgraphCH.hpp"

using namespace std;

class Label {
public:
    int node_id;
    int length;
    Path path;
    int fDist;
    
    Label();
    Label(int node_id, int length) {
        this->node_id = node_id;
        this->length = length;
        this->fDist = 0;
    };
    Label(int node_id, int length, int fDist) {
        this->node_id = node_id;
        this->length = length;
        this->fDist = fDist;
    };
    Label(int node_id, Path path, int length) {
        this->node_id = node_id;
        this->length = length;
        this->path = path;
        this->fDist = 0;
    };
    Label(int node_id, Path path, int length, int fDist) {
        this->node_id = node_id;
        this->length = length;
        this->path = path;
        this->fDist = fDist;
    };
};

class mycomparison {
    bool reverse;
public:
    mycomparison(const bool& revparam=false) {
    	reverse=revparam;
    }
    bool operator() (const Label* lhs, const Label* rhs) const {
        return (lhs->length>rhs->length);
    }
};


class mycomparison_as {
    bool reverse;
public:
    mycomparison_as(const bool& revparam=false) {
    	reverse=revparam;
    }
    bool operator() (const Label* lhs, const Label* rhs) const {
        return (lhs->fDist>rhs->fDist);
    }
};


typedef priority_queue<Label*,std::vector<Label*>,mycomparison> PriorityQueue;
typedef priority_queue<Label*,std::vector<Label*>,mycomparison_as> PriorityQueueAs;

/*
	Definitions
*/

typedef vector<bool> BordersMap; // Size=N, value returns whether the node is a border or not (we use to such maps for incoming and outgoing)
typedef vector<vector<int>> BordersStore; // Size=|components|, size of each set varies. Each set contains the id's of the borders ordered by value
typedef vector<vector<vector<int>>> ComponentDistanceMatrix; // Size=|components|x|components|x(|B_inc(C) x B_out(C)|
typedef vector<vector<bool>> CompExtensionsMap; // Size=N, size of each vector=|components|, each value shows whether the node is in a given extended component.
typedef vector<vector<int>> DistanceTable; // Size=N, size of each vector=[node.component.borders.size], these values are the distances from one node to all the borders of the same component

/*
	Classes for modeling the algorithm
*/

class ParDiSP {
	RoadNetwork *rN;
	int numComponents;
	
	// Structures required by ParDiSP
	ComponentsMap cm;
	CompExtensionsMap cem;
	ComponentDistanceMatrix cdm;
	BordersMap incBordersMap; BordersMap outBordersMap;
	BordersStore incBordersStore; BordersStore outBordersStore;
	DistanceTable incIDT; DistanceTable outIDT; // Distances to/from each node from/to each outgoing border node 
	SubgraphCH *transitNet;

	// Preprocessing
	void preprocessing();
	set<int> get_paths_union(unordered_set<int> &sources, unordered_set<int> &targets);
	set<pair<int,int>> computeIDTOutTransitPart(int component, unordered_set<int> &sources, vector<int> &targets);
	void computeIDTInc(int component, unordered_set<int> &sources, vector<int> &targets);
	void computeCDMWithCH(vector<NodeID> &transitNodes);
	
	// In-component queries
	double ALT(int source, int target);
	pair<Path, double> ALT_Path(RoadNetwork *rN, int source, int target);

	// Cross-component queries	
	int tripleDistanceJoin(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry);
	pair<pair<int,int>,int> tripleDistanceJoinBorders(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry, int srcComp, int trgComp);
	Path nodeToBorderRetrieval(int source, int targetBorder); // NOT IMPLEMENTED YET
	Path borderToNodeRetrieval(int sourceBorder, int target); // NOT IMPLEMENTED YET
	
	// Public constructor and query processing methods
public:
	ParDiSP(RoadNetwork *rN, ComponentsMap &cm, int components);
	~ParDiSP() {};
	int distance(int source, int target);
	pair<Path,int> shortest_path(int source, int target);
};

/* TESTING */
pair<Path,double> Dijkstra(RoadNetwork *rN, int source, int target);
pair<Path, double> DijkstraLimited(RoadNetwork *rN, int source, int target, vector<int> &cm, vector<vector<bool>> &cemEntry);

#endif