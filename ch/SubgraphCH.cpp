#include "SubgraphCH.hpp"
#include "datastr/graph/path.h"

SubgraphCH::SubgraphCH() {
	cout << "Not implemented yet!" << endl;
}

SubgraphCH::SubgraphCH(vector<pair<pair<int,int>,int>> &edgesWithWeights, unordered_map<int,int> &mapOrigToTransit, vector<int> &mapTransitToOrig) {
	
	this->mapOrigToTransit = mapOrigToTransit;
	this->mapTransitToOrig = mapTransitToOrig;
	
	//cout << "[CONSTRUCTOR] mapOrigToTransit size = " << mapOrigToTransit.size() << endl;
	//cout << "[CONSTRUCTOR] mapTransitToOrig size = " << mapTransitToOrig.size() << endl;
	
	vector<CompleteEdge> transitEdgesList;
	transitEdgesList.reserve(edgesWithWeights.size());
	//cout << "--------------------" << endl;
	//cout << mapOrigToTransit.size() << " " << edgesWithWeights.size() << endl;
	for(int i=0;i<edgesWithWeights.size();i++) {
		
		NodeID src = mapOrigToTransit[edgesWithWeights[i].first.first];
		NodeID trg = mapOrigToTransit[edgesWithWeights[i].first.second];
		NodeID weight = edgesWithWeights[i].second;
		
		//cout << src << " " << trg << " " << weight << " 0" << endl;
				
		CompleteEdge edge1(src, trg, weight, false, true, true);
    	CompleteEdge edge2(trg, src, weight, false, true, true);
    	transitEdgesList.push_back(edge1);
    	transitEdgesList.push_back(edge2);
	}
	//cout << "--------------------" << endl;
	vector<CompleteEdge> transitEdgesListNoDuplicates;
	removeDuplicates(transitEdgesList,transitEdgesListNoDuplicates);
	
	vector<LevelID> nodeLevels(mapOrigToTransit.size(), mapOrigToTransit.size());
	//cout << "[SubgraphCH] " << transitEdgesList.size() << " - " << transitEdgesListNoDuplicates.size() << endl;
	
	//for(int i=0;i<transitEdgesListNoDuplicates.size())
	
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
    
    //DijkstraSearchBidir dijkstra(this->updGraph);
    //int dist = dijkstra.bidirSearch(7,259);
    //dijkstra.clear();
    
    //cout << "[CONSTRUCTOR] test_d=" << dist << endl;
    
    //cout << "[CONSTRUCTOR] updGraph nodes = " << this->updGraph->noOfNodes() << endl;
	//cout << "[CONSTRUCTOR] updGraph edges = " << this->updGraph->noOfEdges() << endl;
}

Matrix<EdgeWeight> SubgraphCH::manyToMany(vector<NodeID> sources, vector<NodeID> targets) {
	//cout << "----- manyToMany -----" << endl;
	//cout << "[manyToMany] updGraph nodes = " << this->updGraph->noOfNodes() << endl;
	//cout << "[manyToMany] updGraph edges = " << this->updGraph->noOfEdges() << endl;
	vector<NodeID> sourcesMapped;
	vector<NodeID> targetsMapped;
	
	//cout << "[manyToMany]" << sources.size() << " - " << sourcesMapped.size() << endl;
	//cout << "[manyToMany]" << targets.size() << " - " << targetsMapped.size() << endl;
	//cout << this->mapOrigToTransit.size() << endl;
	
	for(int i=0;i<sources.size();i++) 
		sourcesMapped.push_back(this->mapOrigToTransit[sources[i]]);
		
	for(int i=0;i<targets.size();i++) 
		targetsMapped.push_back(this->mapOrigToTransit[targets[i]]);
	
	//cout << "[manyToMany]" << sources.size() << " - " << sourcesMapped.size() << endl;
	//cout << "[manyToMany]" << targets.size() << " - " << targetsMapped.size() << endl;
	
	//cout << "[manyToMany]" << sources[0] << " - " << mapOrigToTransit[sources[0]] << endl;
	//cout << "[manyToMany]" << targets[20] << " - " << mapOrigToTransit[targets[20]] << endl;
	
	ManyToMany<datastr::graph::UpdateableGraph, DijkstraCHManyToManyFW2, DijkstraCHManyToManyBW2, true> mtm(this->updGraph, 10);
    Matrix<EdgeWeight> matrix(sourcesMapped.size(), targetsMapped.size());
    mtm.computeMatrix(sourcesMapped, targetsMapped, matrix);
    
    /*
    cout << UINT_MAX << endl;
    
    for(int i=0;i<matrix.noOfRows();i++) {
    	for(int j=0;j<matrix.noOfCols();j++) {
    		cout << matrix.value(i,j) << endl;
    	}
    }
    */
    return matrix;
}

void SubgraphCH::get_shortest_path(int source, int target) {
	PathCH p(source);
	//cout << source << " - " << target << endl;
	//if(this->updGraph == NULL) {
	//	cout << "WOOOWWW" << endl;
	//	exit(1);
	//}
	
	//cout << this->mapOrigToTransit[source] << " | " << this->mapOrigToTransit[target] << endl;
	
	DijkstraSearchBidir dijkstra(this->updGraph);
	dijkstra.bidirSearch(this->mapOrigToTransit[source], this->mapOrigToTransit[target]);
	dijkstra.pathTo(p, this->mapOrigToTransit[target]);
    dijkstra.clear();
}
