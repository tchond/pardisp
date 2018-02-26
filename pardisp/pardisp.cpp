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
	this->preprocessing();
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
		set<int> extComp = get_paths_union(bordersOutMap[i], bordersIncMap[i]);
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
	
	vector<int> mapOrigToTransit(this->rN->numNodes, -1);
	vector<int> mapTransitToOrig;
	int t_count = 0;
	for(int i=0;i<transitGraphNodes.size();i++) {
		if(transitGraphNodes[i]) {
			mapOrigToTransit[i] = t_count;
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

int ParDiSP::distance(int source, int target) {
	int srcComp = this->cm[source];
	int trgComp = this->cm[target];
	
	int dist = -1;
	
	if(srcComp != trgComp) {
		dist = this->tripleDistanceJoin(this->outIDT[source], this->incIDT[target], this->cdm[srcComp][trgComp]);
	}
	else {
		dist = this->ALT(source,target);
	}
	return dist;
}

/*
	shortest_path
	-----
	Return the shortest path from the source to the target.
*/

pair<Path,int> ParDiSP::shortest_path(int source, int target) {
	pair<Path,int> result;
	
	int srcComp = this->cm[source];
	int trgComp = this->cm[target];
	
	pair<pair<int,int>,int> borders;
	result.second = 0;
	
	if(srcComp != trgComp) {
		borders = this->tripleDistanceJoinBorders(this->outIDT[source], this->incIDT[target], this->cdm[srcComp][trgComp], srcComp, trgComp);
		int srcBorder = this->outBordersStore[srcComp][borders.first.first];
		int trgBorder = this->incBordersStore[trgComp][borders.first.second];
		
		//cout << source << "(" << this->cm[source] << ") -> " << srcBorder << "(" << this->cm[srcBorder] << ") -> " << trgBorder << "(" << this->cm[trgBorder] << ") -> " << target << "(" << this->cm[target] << ")" << endl;
		
		if(source != srcBorder)	{
			//result.first = this->ALT_Path(source,srcBorder).first;
			
			//for(int i=0;i<result.first.size();i++) {
			//	cout << result.first[i] << " -> ";
			//}
			//cout << endl;
			
			result.first = this->nodeToBorderRetrieval(source,borders.first.first);
			
			//for(int i=0;i<result.first.size();i++) {
			//	cout << result.first[i] << " -> ";
			//}
			//cout << endl;
		}

		vector<NodeID> secondSegment = this->transitNet->get_shortest_path(srcBorder, trgBorder);
		for(int i=1;i<secondSegment.size()-1;i++) {
			result.first.push_back(this->transitNet->mapTransitToOrig[secondSegment[i]]);
		}

		//vector<int> thirdSegment = this->ALT_Path(trgBorder, target).first;
		
		//for(int i=0;i<thirdSegment.size();i++) {
		//	cout << thirdSegment[i] << " -> ";
		//}
		//cout << endl;
		
		vector<int> thirdSegment = this->borderToNodeRetrieval(borders.first.second, target);
		
		//for(int i=0;i<thirdSegment.size();i++) {
		//	cout << thirdSegment[i] << " -> ";
		//}
		//cout << endl;
		result.first.insert(result.first.end(),thirdSegment.begin(), thirdSegment.end());
		
		result.second = borders.second;
	}
	else {
		result = this->ALT_Path(source,target);
	}
	
	return result;
}

/*
	Dijkstra's algorithm for testing purposes
*/

pair<Path, double> Dijkstra(RoadNetwork *rN, int source, int target) {
    
    PriorityQueue Q;
    Path resPath, newPath;
    double newLength = 0,     resLength = 0;
    std::vector<bool> visited(rN->numNodes, false);
    
    newPath.push_back(source);
    vector<Label*> allCreatedLabels;
    
    Label *label = new Label(source, newPath, newLength);
    Q.push(label);
    allCreatedLabels.push_back(label);
    
    while (!Q.empty())     {
        Label *curLabel = Q.top();
        Q.pop();
        
        if (visited[curLabel->node_id])
            continue;
        
        visited[curLabel->node_id] = true;
        
        // Found target.
        if (curLabel->node_id == target) {
            resPath = curLabel->path;
            resLength = curLabel->length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel->node_id].begin(); iterAdj != rN->adjListOut[curLabel->node_id].end(); iterAdj++) {
                newLength = curLabel->length + iterAdj->second;
                newPath = curLabel->path;
                newPath.push_back(iterAdj->first);
                if (!visited[iterAdj->first]) {
                	Label *tlabel = new Label(iterAdj->first, newPath, newLength);
                    Q.push(tlabel);
                    allCreatedLabels.push_back(tlabel);
                }
            }
        }
    }
    
    int visitedCount = 0;
    for(int i=0;i<visited.size();i++) {
    	if(visited[i])
    		visitedCount++;
    }
    //cout << "Dijkstra visited = " << visitedCount << endl; 
    
    for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
    
    return make_pair(resPath, resLength);
}

pair<Path, double> DijkstraLimited(RoadNetwork *rN, int source, int target, vector<int> &cm, vector<vector<bool>> &cem) {
        
    PriorityQueue Q;
    Path resPath, newPath;
    double newLength = 0,     resLength = 0;
    std::vector<bool> visited(rN->numNodes, false);
    vector<Label*> allCreatedLabels;
    
    newPath.push_back(source);
    
    Label *label = new Label(source, newPath, newLength);
    Q.push(label);
    allCreatedLabels.push_back(label);
    
    while (!Q.empty())     {
        Label *curLabel = Q.top();
        Q.pop();
        
        if (visited[curLabel->node_id])
            continue;
        
        visited[curLabel->node_id] = true;
        
        // Found target.
        if (curLabel->node_id == target) {
            resPath = curLabel->path;
            resLength = curLabel->length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel->node_id].begin(); iterAdj != rN->adjListOut[curLabel->node_id].end(); iterAdj++) {
                newLength = curLabel->length + iterAdj->second;
                newPath = curLabel->path;
                newPath.push_back(iterAdj->first);
                if (!visited[iterAdj->first] && (cm[iterAdj->first] == cm[source] || cem[iterAdj->first][cm[source]])) {
                	Label *tlabel = new Label(iterAdj->first, newPath, newLength);
                    Q.push(tlabel);
                    allCreatedLabels.push_back(tlabel);
                }
            }
        }
    }
    
    cout << "Dijkstra Limited labels = " << allCreatedLabels.size() << endl; 
    for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
    
    return make_pair(resPath, resLength);
}


