#ifndef ALGOS_HPP
#define ALGOS_HPP

#include "../graph/graph.hpp"

#include <unordered_set>
#include <cmath> 

pair<Path,double> Dijkstra(RoadNetwork *rN, int source, int target);
vector<int> DijkstraToAll(RoadNetwork *rN, int source);
vector<int> DijkstraFromAll(RoadNetwork *rN, int target);
vector<double> DijkstraToMany(RoadNetwork *rN, int source, unordered_set<int> targets);
pair<Path,double> DijkstraRev(RoadNetwork *rN, int source, int target);

pair<Path, double> DijkstraLimited(RoadNetwork *rN, int source, int target, vector<int> &cm, vector<vector<bool>> &cemEntry);


pair<Path, double> ALT(RoadNetwork *rN, int source, int target, vector<int> &fromLandmark, vector<int> &toLandmark);

// This function is used to compute the component extension
set<int> getPathsUnion(RoadNetwork *rN, unordered_set<int> &sources, unordered_set<int> &targets);

vector<pair<int,int>> DijkstraFromMany(RoadNetwork *rN, int source, vector<bool> &targets, vector<bool> &transitGraphNodes, int limit);
int tripleDistanceJoin(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry);
pair<int,int> tripleDistanceJoinBorders(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry);

class Label {
public:
    int node_id;
    int length;
    Path path;
    int fDist;
    
    Label();
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
    bool operator() (const Label& lhs, const Label&rhs) const {
        return (lhs.length>rhs.length);
    }
};

class mycomparison_as {
    bool reverse;
public:
    mycomparison_as(const bool& revparam=false) {
    	reverse=revparam;
    }
    bool operator() (const Label& lhs, const Label&rhs) const {
        return (lhs.fDist>rhs.fDist);
    }
};

typedef priority_queue<Label,std::vector<Label>,mycomparison> PriorityQueue;
typedef priority_queue<Label,std::vector<Label>,mycomparison_as> PriorityQueueAs;

#endif