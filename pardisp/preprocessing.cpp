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
 
 #include "pardisp.hpp"

/*
	This method is used to compute the extended components.
*/

set<int> ParDiSP::get_paths_union(unordered_set<int> &sources, unordered_set<int> &targets) {
	set<int> nodesUnion;
	
	vector<Label*> allCreatedLabels;
	
	for (unordered_set<int>::iterator it = sources.begin(); it != sources.end(); it++) {
		int source = *it;
		int resultSize = 0;
		PriorityQueue Q;
   		Path resPath, newPath;
    	double newLength = 0, resLength = 0;
    	std::vector<bool> visited(this->rN->numNodes, false);
    
    	newPath.push_back(source);
    	
    	Label *label = new Label(source, newPath, newLength);
    	Q.push(label);
    	allCreatedLabels.push_back(label);
    	
    	while (!Q.empty())     {
        	Label *curLabel = Q.top();
        	Q.pop();
        	
        	if (visited[curLabel->node_id])
            	continue;
      
        	if (targets.find(curLabel->node_id) != targets.end()) {
        		for(int i=0;i<curLabel->path.size();i++)
            		nodesUnion.insert(curLabel->path[i]);
            	resLength = curLabel->length;
            	resultSize++;
            	if(resultSize == targets.size())
            		break;
        	}
        
        	visited[curLabel->node_id] = true;
    
            for (EdgeList::iterator iterAdj = this->rN->adjListOut[curLabel->node_id].begin(); iterAdj != this->rN->adjListOut[curLabel->node_id].end(); iterAdj++) {
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
	
	for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
	
	return nodesUnion;
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
    	Q.push(new Label(target, newPath, newLength));
    	
    	while (!Q.empty())     {
        	Label *curLabel = Q.top();
        	Q.pop();
        	
        	if (visited[curLabel->node_id])
            	continue;
      
      		distances[curLabel->node_id] = curLabel->length;
      
        	if (sources.find(curLabel->node_id) != sources.end()) {
        		for(int i=0;i<curLabel->path.size()-1;i++) {
        			auto v = result.insert(make_pair(curLabel->path[i], curLabel->path[i+1]));
        			//std::cout << v.second << "\n";
        		}
        	}
        
        	visited[curLabel->node_id] = true;
    
            for (EdgeList::iterator iterAdj = rN->adjListInc[curLabel->node_id].begin(); iterAdj != rN->adjListInc[curLabel->node_id].end(); iterAdj++) {
               	newLength = curLabel->length + iterAdj->second;
               	newPath = curLabel->path;
               	newPath.push_back(iterAdj->first);
               	if (!visited[iterAdj->first] &&  (this->cem[iterAdj->first][cm[target]] || this->cm[target]==this->cm[iterAdj->first])) // expanding only component extension
                //if (!visited[iterAdj->first])
                   	Q.push(new Label(iterAdj->first, newPath, newLength));
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
    	Q.push(new Label(target, newPath, newLength));
    	
    	while (!Q.empty())     {
        	Label *curLabel = Q.top();
        	Q.pop();
        	
        	if (visited[curLabel->node_id])
            	continue;
      
      		distances[curLabel->node_id] = curLabel->length;
              
        	visited[curLabel->node_id] = true;
    
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel->node_id].begin(); iterAdj != rN->adjListOut[curLabel->node_id].end(); iterAdj++) {
               	newLength = curLabel->length + iterAdj->second;
               	newPath = curLabel->path;
               	newPath.push_back(iterAdj->first);
                if (!visited[iterAdj->first] && (this->cem[iterAdj->first][cm[target]] || this->cm[target]==this->cm[iterAdj->first])) // expanding only component extension
                   	Q.push(new Label(iterAdj->first, newPath, newLength));
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
