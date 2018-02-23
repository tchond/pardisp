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

#include "SubgraphCH.hpp"
#include "datastr/graph/path.h"

SubgraphCH::SubgraphCH() {
	cout << "Not implemented yet!" << endl;
}

SubgraphCH::SubgraphCH(vector<pair<pair<int,int>,int>> &edgesWithWeights, unordered_map<int,int> &mapOrigToTransit, vector<int> &mapTransitToOrig) {
	
	this->mapOrigToTransit = mapOrigToTransit;
	this->mapTransitToOrig = mapTransitToOrig;
	
	
	vector<CompleteEdge> transitEdgesList;
	transitEdgesList.reserve(edgesWithWeights.size());
	for(int i=0;i<edgesWithWeights.size();i++) {
		
		NodeID src = mapOrigToTransit[edgesWithWeights[i].first.first];
		NodeID trg = mapOrigToTransit[edgesWithWeights[i].first.second];
		NodeID weight = edgesWithWeights[i].second;
						
		CompleteEdge edge1(src, trg, weight, false, true, true);
    	CompleteEdge edge2(trg, src, weight, false, true, true);
    	transitEdgesList.push_back(edge1);
    	transitEdgesList.push_back(edge2);
	}
	vector<CompleteEdge> transitEdgesListNoDuplicates;
	removeDuplicates(transitEdgesList,transitEdgesListNoDuplicates);
	
	vector<LevelID> nodeLevels(mapOrigToTransit.size(), mapOrigToTransit.size());
		
	this->updGraph = new datastr::graph::UpdateableGraph(transitEdgesListNoDuplicates, nodeLevels);
	
	// Options for computing the node order
	ProcessingCH::WeightCalculation calc; 
	calc.edgeDiffMult = 190; 				// x
	calc.searchSpaceMult = 1; 				// y
    calc.lazyUpdateRecalcLimit = 1000; 		// p 
    calc.maxSettledApprox = 1000; 			// n 
    calc.voronoiMult = 60; 					// V
    calc.searchPathHopBorderMult = 145; 	// S
    calc.shortcutOriginalEdgeSumMult = 70; 	// e
    ProcessingCH::ContractParameters contractParams;
    
    ProcessingCH c(this->updGraph);
    c.createHierarchy(calc, NULL, NULL, "", false);
	c.constructHierarchy(contractParams);
    c.clear();
    
}

Matrix<EdgeWeight> SubgraphCH::manyToMany(vector<NodeID> sources, vector<NodeID> targets) {
	vector<NodeID> sourcesMapped;
	vector<NodeID> targetsMapped;
	
	for(int i=0;i<sources.size();i++) 
		sourcesMapped.push_back(this->mapOrigToTransit[sources[i]]);
		
	for(int i=0;i<targets.size();i++) 
		targetsMapped.push_back(this->mapOrigToTransit[targets[i]]);
	
	ManyToMany<datastr::graph::UpdateableGraph, DijkstraCHManyToManyFW2, DijkstraCHManyToManyBW2, true> mtm(this->updGraph, 10);
    Matrix<EdgeWeight> matrix(sourcesMapped.size(), targetsMapped.size());
    mtm.computeMatrix(sourcesMapped, targetsMapped, matrix);
    
    return matrix;
}

int SubgraphCH::get_shortest_path(int source, int target) {
	PathCH p(source);	
	DijkstraSearchBidir dijkstra(this->updGraph);
	int dist = dijkstra.bidirSearch(this->mapOrigToTransit[source], this->mapOrigToTransit[target]);
	dijkstra.pathTo(p, this->mapOrigToTransit[target]);
    dijkstra.clear();
    return dist;
}
