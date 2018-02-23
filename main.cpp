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
 
 
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "graph/graph.hpp"
#include "preprocessing/algos.hpp"
#include "preprocessing/pardisp.hpp"

using namespace std;

int main(int argc, char **argv) {
    
    // Our graph
    RoadNetwork *rN = 0;
    
    // Partition file
    string graphFile = "";
    string partitionFile = "";
    string extCompFile = "";
    string queryPrefix = "";
       
   	int opt = -1;
	while ((opt = getopt(argc, argv, "f:p:e:q:")) != -1) {
		switch(opt) {
			case 'f':
				graphFile = string(optarg);
				break;
			case 'p':
				partitionFile = string(optarg);
				break;
		}
	}
	
	if(graphFile == "") {
    	cerr << "Wrong values for the arguments" << endl;
    	exit(0);
    }
	    
    rN = new RoadNetwork(graphFile.c_str());
    
	cerr << "Nodes = " << rN->numNodes << ", Edges = " << rN->numEdges << endl;
    
	/*
		PREPROCESSING
	*/
	
	int dist;
	
	pair<ComponentsMap,int> mapPair = loadComponents(rN,partitionFile);
	ParDiSP algo(rN,mapPair.first, mapPair.second);
		
	algo.preprocessing();
    
	/*
		DISTANCE QUERIES
	 */
    int problems = 0;
	cout << "----- DIST -----" << endl;
	for(int i=1;i<=100;i++) {
    	int src = (int) (rand() % rN->numNodes);
    	int trg = (int) (rand() % rN->numNodes);
    	dist = algo.distance(src,trg);
    	//cout << i << ".\t" << src << "\t" << trg << "\t" << dist << endl;
        int dist2 = Dijkstra(rN,src,trg).second;
        if(mapPair.first[src] == mapPair.first[trg])       
        	cout << src << "\t" << trg << "\t" << dist << "\t" << dist2 << endl;
        if(dist != dist2)
            problems++;
    }
    cout << "PROBLEMS: " << problems << endl;
  	
    delete rN;
    return 0;
}

// doesn't look nice, but required by the compiler (gcc 4)
const EdgeWeight Weight::MAX_VALUE;
const EliminationWeight::Type EliminationWeight::MAX_VALUE = __DBL_MAX__;
const EliminationWeight::Type EliminationWeight::MIN_VALUE = -__DBL_MAX__;
const EdgeWeight Weight::MAX_INTEGER;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_BACKWARD;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_FORWARD;
