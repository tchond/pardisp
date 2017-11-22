#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>

#include <boost/functional/hash.hpp>

//#define VERIFY_GRAPH

using namespace std;

typedef unordered_map<int,int> EdgeList;
typedef vector<int> Path;

class RoadNetwork {
public:
    int numNodes;
    int numEdges;
   	EdgeList *adjListOut;
   	EdgeList *adjListInc;
   	   
    RoadNetwork(const char *filename);
    int getEdgeWeight(int lnode, int rnode);
    void print();
    ~RoadNetwork();
    
    EdgeList ougoingEdgesOf(int);
    EdgeList incomingEdgesOf(int);
    int outDegreeOf(int);
    int incDegreeOf(int);
    set<int> neighborsOf(int);
    
    bool addEdge(int,int,int);
    bool removeEdge(int,int);
};

#endif  