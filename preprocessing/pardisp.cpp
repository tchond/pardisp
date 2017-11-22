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
#include <iomanip>
#include <fstream>
#include <sstream>

#include "pardisp.hpp"
#include "../stats/benchmark.hpp"
#include "algos.hpp"

using namespace std;

ParDiSP::ParDiSP(RoadNetwork *rN, ComponentsMap &cm, int numComponents) {
	this->rN = rN;
	this->cm = cm;
	this->numComponents = numComponents;
	
	this->incBordersMap = vector<bool>(rN->numNodes);
	this->outBordersMap = vector<bool>(rN->numNodes);
	this->incBordersStore = vector<vector<int>>(numComponents);
	this->outBordersStore = vector<vector<int>>(numComponents);
	this->incIDT = DistanceTable(rN->numNodes);
	this->outIDT = DistanceTable(rN->numNodes);
	
	this->cdm = ComponentDistanceMatrix(this->numComponents,vector<vector<int>>(this->numComponents));
}

/*
	In this method the entire preprocessing phase is executed.
*/

void ParDiSP::preprocessing() {
	cerr << "Preprocessing started" << endl;
	double sTime,eTime;
	
	int compSize = outBordersStore.size();
	set<pair<int,int>> transitEdges;
	vector<NodeID> transitNodes;
	vector<bool> transitGraphNodes(rN->numNodes, false);
	
	vector<unordered_set<int>> bordersOutMap(outBordersStore.size());
	vector<unordered_set<int>> bordersIncMap(incBordersStore.size());
	
	cerr << "Populating border containers" << endl;
	
	for(int nid=0;nid<this->rN->numNodes;nid++) {
		this->cem.push_back(vector<bool>(outBordersStore.size(),false));
		this->cem[nid][cm[nid]] = true;
		for (EdgeList::iterator it = rN->adjListOut[nid].begin(); it != rN->adjListOut[nid].end(); it++) {
   			 if(cm[nid] != cm[it->first]) { // this is the check for the connecting edges
   			 	this->outBordersMap[nid] = true;
   			 	this->incBordersMap[it->first] = true;
   			 	bordersOutMap[cm[nid]].insert(nid);
   			 	bordersIncMap[cm[it->first]].insert(it->first);
   			 	transitEdges.insert(make_pair(nid,it->first));
   			 }
   		}
	}

	/*
		Populating the border containers and sorting
	*/
	
	for(int i=0;i<this->outBordersStore.size();i++) {
		for(int nid=0;nid<this->outBordersMap.size();nid++) {
			if(outBordersMap[nid] && cm[nid]==i)
				this->outBordersStore[i].push_back(nid);
		}
		std::sort(outBordersStore[i].begin(),outBordersStore[i].end());
		
		for(int nid=0;nid<this->incBordersMap.size();nid++) {
			if(incBordersMap[nid] && cm[nid]==i)
				this->incBordersStore[i].push_back(nid);
		}
		std::sort(incBordersStore[i].begin(),incBordersStore[i].end());
	}
	
	/*
		Computing extended components
	*/
	
	cerr << "Computing extended components" << endl;
	sTime = GetTimeSec();
	for(int i=0;i<compSize;i++) {
		set<int> extComp = getPathsUnion(rN, bordersOutMap[i], bordersIncMap[i]);
		for(set<int>::iterator it = extComp.begin(); it != extComp.end() ; it++) {
			this->cem[*it][i] = true; // I THINK IT IS HERE!!!
		}	
	}
	eTime = GetTimeSec();
	cout << "DONE with extended components in " << (double)(eTime-sTime) << " seconds" << endl;
	
	/*
		Compute IDTs
	*/
	
	cerr << "Computing IDTs" << endl;
	sTime = GetTimeSec();
	for(int i=0;i<compSize;i++) {
		set<pair<int,int>> transitEdges_temp = this->computeIDTOutTransitPart(i, bordersIncMap[i], outBordersStore[i]);
		transitEdges.insert(transitEdges_temp.begin(), transitEdges_temp.end());
		this->computeIDTInc(i, bordersOutMap[i], incBordersStore[i]);
	}
		
	long idtSize = 0;
	for(int nid=0;nid<this->rN->numNodes;nid++) {
		idtSize += this->outIDT[nid].size();
		assert(this->outIDT[nid].size() == this->outBordersStore[cm[nid]].size());
	}
	for(int nid=0;nid<this->rN->numNodes;nid++) {
		idtSize += this->outIDT[nid].size();
		assert(this->incIDT[nid].size() == this->incBordersStore[cm[nid]].size());
	}
	eTime = GetTimeSec();
	cout << "DONE with IDTs in " << (double)(eTime-sTime) << " seconds" << endl;
	
	int entries = 0;
	for(int i=0;i<this->incIDT.size();i++) {
		entries += this->incIDT[i].size();
	}
	for(int i=0;i<this->outIDT.size();i++) {
		entries += this->outIDT[i].size();
	}
	
	/*
		Populating the nodes for the transit graph
	*/
	
	for (set<pair<int,int>>::iterator it = transitEdges.begin(); it != transitEdges.end(); it++) {
		transitGraphNodes[it->first] = true;
		transitGraphNodes[it->second] = true;
	}
			
	
	vector<pair<pair<int,int>,int>> trEdgesWeights;
		
	for (set<pair<int,int>>::iterator it = transitEdges.begin(); it != transitEdges.end(); it++) {
		pair<int,int> p = *it;
		int w=this->rN->getEdgeWeight(p.first,p.second);
		trEdgesWeights.push_back(make_pair(p,w));
	}
	
	unordered_map<int,int> mapOrigToTransit;
	vector<int> mapTransitToOrig;
	int t_count = 0;
	for(int i=0;i<transitGraphNodes.size();i++) {
		if(transitGraphNodes[i]) {
			mapOrigToTransit.insert(make_pair(i,t_count));
			mapTransitToOrig.push_back(i);
			t_count++;
			transitNodes.push_back(i);
		}
	}
	
	/*
		Compute CDM
	*/
	cerr << "Computing CDM" << endl;
	sTime = GetTimeSec();
	this->transitNet = new SubgraphCH(trEdgesWeights, mapOrigToTransit, mapTransitToOrig);
	this->computeCDMWithCH(transitNodes);
	eTime = GetTimeSec();
	cout << "DONE with CDM in " << (double)(eTime-sTime) << endl;
		
	
	long cdmSize = 0;
	assert(this->numComponents>0);
	for(int i=0;i<this->numComponents;i++) {
		for(int j=0;j<this->numComponents;j++) {
			if(i != j) {				
				cdmSize += cdm[i][j].size();				
			}
		}
	}
	
	int bordersNum = 0;
	int max = 0;
	int min = rN->numNodes;
	for(int i=0;i<this->outBordersStore.size();i++) {
		bordersNum += this->outBordersStore[i].size();
		if(this->outBordersStore[i].size() > max) 
			max = this->outBordersStore[i].size();
		if(this->outBordersStore[i].size() < min) 
			min = this->outBordersStore[i].size();
	}
	
	vector<int> compCount(numComponents,0);
	for(int i=0;i<this->cm.size();i++)
		compCount[this->cm[i]]++;
	int maxPC = 0;
	int minPC = rN->numNodes; 
	int sumPC = 0;
	for(int i=0;i<compCount.size();i++) {
		sumPC += compCount[i];
		if(compCount[i] > maxPC) 
			maxPC = compCount[i];
		if(compCount[i] < minPC) 
			minPC = compCount[i];
	}
	
	vector<int> extCompCount(numComponents,0);
	for(int i=0;i<this->cem.size();i++) {
		for(int j=0;j<numComponents;j++) {
			if(this->cem[i][j])
				extCompCount[j]++;
		}
	}
	
	int maxPEC = 0;
	int minPEC = rN->numNodes; 
	int sumPEC = 0;
	for(int i=0;i<extCompCount.size();i++) {
		sumPEC += extCompCount[i];
		if(compCount[i] > maxPEC) 
			maxPEC = extCompCount[i];
		if(compCount[i] < minPEC) 
			minPEC = extCompCount[i];
	}
	
	cout << "----- STATS -----" << endl;
	cout << "----- " << numComponents << " -----" << endl;
	cout << "Borders (avg) = " << bordersNum/numComponents << endl;
	cout << "Borders (min) = " << min << endl;
	cout << "Borders (max) = " << max << endl;
	cout << "Component size (avg) = " << rN->numNodes/numComponents << endl;
	cout << "Component size (min) = " << minPC << endl;
	cout << "Component size (max) = " << maxPC << endl;
	cout << "Ext. Component size (avg) = " << sumPEC/numComponents << endl;
	cout << "Ext. Component size (min) = " << minPEC << endl;
	cout << "Ext. Component size (max) = " << maxPEC << endl;
	cout << "Transit Network Nodes = " << t_count << endl;
	cout << "Transit Network Edges = " << transitEdges.size() << endl;
	cout << "Memory (IDTs) = " << ((idtSize)*4) << " bytes" << endl;
	cout << "Memory (CDM) = " << ((cdmSize)*4) << " bytes" << endl;
	cout << "Memory = " << ((idtSize+cdmSize)*4) << " bytes" << endl;
	cout << "-----------------" << endl;
	
	cerr << "Done" << endl;
	
	bordersOutMap.clear();
	bordersIncMap.clear();
}

set<pair<int,int>> ParDiSP::computeIDTOutTransitPart(int component, unordered_set<int> &sources, vector<int> &targets){
	set<pair<int,int>> result;
	
	for (int i=0;i<targets.size();i++) {
		int target = targets[i];
		int resultSize = 0;
		vector<int> distances(this->rN->numNodes,INT_MAX);
		PriorityQueue Q;
   		Path resPath, newPath;
    	double newLength = 0, resLength = 0;
    	std::vector<bool> visited(rN->numNodes, false);
    
    	newPath.push_back(target);
    	Q.push(Label(target, newPath, newLength));
    	
    	while (!Q.empty())     {
        	Label curLabel = Q.top();
        	Q.pop();
        	
        	if (visited[curLabel.node_id])
            	continue;
      
      		distances[curLabel.node_id] = curLabel.length;
      
        	if (sources.find(curLabel.node_id) != sources.end()) {
        		for(int i=0;i<curLabel.path.size()-1;i++) {
        			auto v = result.insert(make_pair(curLabel.path[i], curLabel.path[i+1]));
        			//std::cout << v.second << "\n";
        		}
        	}
        
        	visited[curLabel.node_id] = true;
    
            for (EdgeList::iterator iterAdj = rN->adjListInc[curLabel.node_id].begin(); iterAdj != rN->adjListInc[curLabel.node_id].end(); iterAdj++) {
               	newLength = curLabel.length + iterAdj->second;
               	newPath = curLabel.path;
               	newPath.push_back(iterAdj->first);
               	if (!visited[iterAdj->first] &&  (this->cem[iterAdj->first][cm[target]] || this->cm[target]==this->cm[iterAdj->first])) // expanding only component extension
                //if (!visited[iterAdj->first])
                   	Q.push(Label(iterAdj->first, newPath, newLength));
            }
    	}
    	
    	// VERIFY RESULTS
    	#ifdef VERIFY_GRAPH
    	for(int i=0;i<distances.size();i++) {
    	
    		pair<Path,int> my_pair = Dijkstra(rN, i, target);
    		if(distances[i] < INT_MAX && cm[i] == cm[target]) {
    			cout << i << " -> " << target << ") dist=" <<  distances[i] << endl;
    			assert(distances[i] == my_pair.second);
    		}
    	}
    	#endif
    	
    	// POPULATE
    	for(int i=0;i<distances.size();i++) {
    		if(distances[i] < INT_MAX && cm[i] == cm[target]) {
				this->outIDT[i].push_back(distances[i]);
    		}
    	}
    	
    	
	}
	#ifdef VERIFY_GRAPH
	for (set<pair<int,int>>::iterator it = result.begin(); it != result.end(); it++) {
		pair<int,int> edge = *it;
		cout << edge.first << " - " << edge.second << endl;
	}
	#endif
	
	return result;
}

void ParDiSP::computeIDTInc(int component, unordered_set<int> &sources, vector<int> &targets) {
	for (int i=0;i<targets.size();i++) {
		int target = targets[i];
		int resultSize = 0;
		vector<int> distances(this->rN->numNodes,INT_MAX);
		PriorityQueue Q;
   		Path resPath, newPath;
    	double newLength = 0, resLength = 0;
    	std::vector<bool> visited(rN->numNodes, false);
    
    	newPath.push_back(target);
    	Q.push(Label(target, newPath, newLength));
    	
    	while (!Q.empty())     {
        	Label curLabel = Q.top();
        	Q.pop();
        	
        	if (visited[curLabel.node_id])
            	continue;
      
      		distances[curLabel.node_id] = curLabel.length;
              
        	visited[curLabel.node_id] = true;
    
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel.node_id].begin(); iterAdj != rN->adjListOut[curLabel.node_id].end(); iterAdj++) {
               	newLength = curLabel.length + iterAdj->second;
               	newPath = curLabel.path;
               	newPath.push_back(iterAdj->first);
                if (!visited[iterAdj->first] && (this->cem[iterAdj->first][cm[target]] || this->cm[target]==this->cm[iterAdj->first])) // expanding only component extension
                   	Q.push(Label(iterAdj->first, newPath, newLength));
            }
    	}
    	
    	// POPULATE
    	
    	for(int i=0;i<distances.size();i++) {
    		if(distances[i] < INT_MAX && cm[i] == cm[target]) {
				this->incIDT[i].push_back(distances[i]);
    		}
    	}
	}
}

void ParDiSP::computeCDMWithCH(vector<NodeID> &transitNodes) {
	unordered_map<int,int> tnMatrixIdxMap;
	vector<NodeID> sources;
	vector<NodeID> targets;
	
	int count = 0;
	for(int i=0;i<transitNodes.size();i++) {
		if(this->incBordersMap[transitNodes[i]] || this->outBordersMap[transitNodes[i]])
			tnMatrixIdxMap.insert(make_pair(transitNodes[i],count++));
		if(this->incBordersMap[transitNodes[i]])
			sources.push_back(transitNodes[i]);
		if(this->outBordersMap[transitNodes[i]])
			targets.push_back(transitNodes[i]);
	}
	
	Matrix<EdgeWeight> matrix = this->transitNet->manyToMany(sources,targets);

	for(int i=0;i<transitNodes.size();i++) {	
		if(!this->outBordersMap[transitNodes[i]])
			continue;
		int tComp = this->cm[transitNodes[i]];
		int trgIdx = tnMatrixIdxMap[transitNodes[i]];
		for(int j=0;j<transitNodes.size();j++) {
			int sComp = this->cm[transitNodes[j]];
			if(!this->incBordersMap[transitNodes[j]] || sComp==tComp)
				continue;
			int srcIdx = tnMatrixIdxMap[transitNodes[j]];
			
			this->cdm[sComp][tComp].push_back(matrix.value(srcIdx,trgIdx));
		}
	}
}

int ParDiSP::distance(int source, int target) {
	int srcComp = this->cm[source];
	int trgComp = this->cm[target];
	
	int dist = -1;
	
	if(srcComp != trgComp) {
		dist = tripleDistanceJoin(this->outIDT[source], this->incIDT[target], this->cdm[srcComp][trgComp]);
	}
	else {
		dist = DijkstraLimited(this->rN,source,target,this->cm,this->cem).second;
	}
	return dist;
}

/*
	shortest_path
	-----
	Return the shortest path from the source to the target.
*/
Path ParDiSP::shortest_path(int source, int target) {
	Path path;
	path.push_back(source);
	
	int srcComp = this->cm[source];
	int trgComp = this->cm[target];
	
	pair<int,int> borders;
	int dist = -1;
	
	if(srcComp != trgComp) {
		borders = tripleDistanceJoinBorders(this->outIDT[source], this->incIDT[target], this->cdm[srcComp][trgComp]);
		if(this->transitNet == NULL) {
			cout << "Another WOW" << endl;
			exit(1);
		}
		this->transitNet->get_shortest_path(this->outBordersStore[srcComp][borders.first], this->incBordersStore[trgComp][borders.second]);
	}
	else {
		dist = DijkstraLimited(this->rN,source,target,this->cm,this->cem).second;
	}
	
	path.push_back(target);
	return path;
}
