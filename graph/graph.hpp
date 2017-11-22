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