/* Copyright (C) 2005, 2006, 2007, 2008 
 * Robert Geisberger, Dominik Schultes, Peter Sanders,
 * Universitaet Karlsruhe (TH)
 *
 * This file is part of Contraction Hierarchies.
 *
 * Contraction Hierarchies is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * Contraction Hierarchies is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Contraction Hierarchies; see the file COPYING; if not,
 * see <http://www.gnu.org/licenses/>.
 */

using namespace std;

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "config.h"

#include "stats/counter.h"
#include "stats/utils.h"

Counter counter;

#include "Command.h"
#include "io/createGraph.h"
#include "io/output.h"
#include "datastr/graph/UpdateableGraph.h"
#include "datastr/graph/SearchGraph.h"
#include "processing/DijkstraCH.h"
#include "processing/ConstructCH.h"

typedef processing::ConstructCH<datastr::graph::UpdateableGraph> ProcessingNodeOrder;
typedef processing::ConstructCH<datastr::graph::UpdateableGraph> ProcessingConstruct;

void node_order(string file, string output) {	
	ProcessingNodeOrder::WeightCalculation calc; 
	
	calc.edgeDiffMult = 190; 				// x
	calc.searchSpaceMult = 1; 				// y
    calc.lazyUpdateRecalcLimit = 1000; 		// p 
    calc.maxSettledApprox = 1000; 			// n 
    calc.voronoiMult = 60; 					// V
    calc.searchPathHopBorderMult = 145; 	// S
    calc.shortcutOriginalEdgeSumMult = 70; 	// e
	
	// Read original graph. Each node will be in level n. During the node ordering
    // each contratacted node will be put in its own level.
    std::ifstream in(file.c_str());
    if (!in.is_open()) { cerr << "Cannot open " << file << endl; exit(1); }
    datastr::graph::UpdateableGraph* graph = importGraphListOfEdgesUpdateable(in, false, false, "");
    in.close();
    
    srand(22);

    // Object for node ordering
    ProcessingNodeOrder c(graph);
    c.createHierarchy(calc, NULL, NULL, "", false);
    
    ofstream out(output.c_str());
    if (!out.is_open()) { cerr << "Cannot write to " << output << endl; exit(1); }
    c.writeLevels(out);
    out.close();
    
}

datastr::graph::UpdateableGraph* construct(string file, string hcnFile) {

	ProcessingNodeOrder::WeightCalculation calc; 
	
	calc.edgeDiffMult = 190; 				// x
	calc.searchSpaceMult = 1; 				// y
    calc.lazyUpdateRecalcLimit = 1000; 		// p 
    calc.maxSettledApprox = 1000; 			// n 
    calc.voronoiMult = 60; 					// V
    calc.searchPathHopBorderMult = 145; 	// S
    calc.shortcutOriginalEdgeSumMult = 70; 	// e

    ProcessingConstruct::ContractParameters contractParams;
    
    // Read original graph.
    std::ifstream in(file.c_str());
    if (!in.is_open()) { cerr << "Cannot open " << file << endl; exit(1); }
    datastr::graph::UpdateableGraph* updGraph = importGraphListOfEdgesUpdateable(in, false, false, "");
    in.close();
    
    //ProcessingConstruct construct(updGraph);
    
    //construct.createHierarchy(calc, NULL, NULL, "", false);
    
    // read in node order from hcn-file
    //in.open(hcnFile.c_str());
    //if (!in.is_open()) { cerr << "Cannot open " << hcnFile << endl; exit(1); }
    //construct.readLevels(in);
    //in.close();
    
   	//construct.constructHierarchy(contractParams);
    //construct.clear();
        
    return updGraph;
}

datastr::graph::UpdateableGraph* allInOne(string file, string hcnFile) {

    ProcessingNodeOrder::WeightCalculation calc; 
	
	calc.edgeDiffMult = 190; 				// x
	calc.searchSpaceMult = 1; 				// y
    calc.lazyUpdateRecalcLimit = 1000; 		// p 
    calc.maxSettledApprox = 1000; 			// n 
    calc.voronoiMult = 60; 					// V
    calc.searchPathHopBorderMult = 145; 	// S
    calc.shortcutOriginalEdgeSumMult = 70; 	// e

    ProcessingConstruct::ContractParameters contractParams;
    
    // Read original graph.
    std::ifstream in(file.c_str());
    if (!in.is_open()) { cerr << "Cannot open " << file << endl; exit(1); }
    datastr::graph::UpdateableGraph* updGraph = importGraphListOfEdgesUpdateable(in, false, false, "");
    in.close();
    
    ProcessingConstruct construct(updGraph);
    
    construct.createHierarchy(calc, NULL, NULL, "", false);
    
    // read in node order from hcn-file
    //in.open(hcnFile.c_str());
    //if (!in.is_open()) { cerr << "Cannot open " << hcnFile << endl; exit(1); }
    //construct.readLevels(in);
    //in.close();
    
   	construct.constructHierarchy(contractParams);
    construct.clear();
        
    return updGraph;
}

/** The main program. */
int main(int argc, char *argv[]) {
    // The program is controlled by command-line arguments. The order of those
    // arguments is important. The first argument specififies the the Command-
    // class that is used.
    int opt = -1;
    int m = -1;
    int result = 0;
    
    string file = "";
    string output = "";
    string logFile = "";
    string hcnFile = "";
    string queryFile = "";
    
    while ((opt = getopt(argc, argv, "m:f:o:l:h:q:")) != -1) {
    	switch (opt) {
        	case 'm':
        		m = atoi(optarg);
        	    break;
       	 	case 'f':
            	file = string(optarg);
            	break;
        	case 'o':
            	output = string(optarg);
            	break;
            case 'h':
            	hcnFile = string(optarg);
            	break;
            case 'q':
            	queryFile = string(optarg);
            	break;   
    	}
	}
	if (m == 1) {
		double startTime = timestamp();
		node_order(file,output);
		double stopTime = timestamp();
		cout << "Preprocessing time = " << (stopTime-startTime)/1000000 << " seconds." << endl;
		cout << "-------------------" << endl;
    }
    else if (m == 2){
    	double startTime = timestamp();
    	construct(file,hcnFile);
    	double stopTime = timestamp();
    	cout << "Preprocessing time = " << (stopTime-startTime)/1000000 << " seconds." << endl;
		cout << "-------------------" << endl;
		

		
    }
    else if (m==3) {
    	double startTimePre = timestamp();
    	datastr::graph::UpdateableGraph* updGraph = allInOne(file,hcnFile);
    	double stopTimePre = timestamp();
    	cout << "Preprocessing time = " << (stopTimePre-startTimePre)/1000000 << " seconds." << endl;
		cout << "-------------------" << endl;

		DijkstraSearchBidir dijkstra(updGraph);
		cout << "test dist = " << dijkstra.bidirSearch(7, 259) << endl;
    	dijkstra.clear();
		/*
    	DijkstraSearchBidir dijkstra(updGraph);
    	    	
    	int i = 0;
		for(i=0;i<10;i++) {
			char set[10];
			sprintf(set,"d_%d.txt",i);
			char fpath[128] = "";
			strcat(fpath, queryFile.c_str());
			strcat(fpath, set);
			cout << "testing with " << fpath << endl;
			vector<pair<int,int>> queries;

			FILE* ifile = fopen(fpath, "r");
	
			int query_num,x,y;
		
			fscanf( ifile, "%d", &query_num );

			for ( int j = 0 ; j < query_num ; ++j ) {
				fscanf( ifile, "%d %d", &x,&y );
				queries.push_back(make_pair(x,y));
			}

			fclose(ifile);
			
			double startTime = timestamp();
			for ( int j = 0 ; j < query_num ; ++j ) {
				//cout << "now" << queries[j].first << " - " << queries[j].second << endl;
				dijkstra.bidirSearch(queries[j].first, queries[j].second);
    			dijkstra.clear();
			}
			double stopTime = timestamp();
			double distTime = (stopTime-startTime)/queries.size();
			cout << "d_" << i << ") average time =\t" << distTime << endl;
		}
		*/
    }
    else {
        cout << "Not valid mode" << endl;
    }
    
    return result;
}


// doesn't look nice, but required by the compiler (gcc 4)
const EdgeWeight Weight::MAX_VALUE;
const EliminationWeight::Type EliminationWeight::MAX_VALUE = __DBL_MAX__;
const EliminationWeight::Type EliminationWeight::MIN_VALUE = -__DBL_MAX__;
const EdgeWeight Weight::MAX_INTEGER;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_BACKWARD;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_FORWARD;
