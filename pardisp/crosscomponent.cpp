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
 
int ParDiSP::tripleDistanceJoin(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry) {
	
	int minDist = INT_MAX;
	int dist;
	int idx = -1;
	int modulo;
	
	vector<int> interim(idtTarget.size(),INT_MAX);
	
	// use variables and write them to the array only in the end.
	
	for(int i=0;i<cdmEntry.size();i++) {
		modulo = i%idtSource.size();
		if(modulo == 0) 
			idx++;
		dist = cdmEntry[i] + idtSource[modulo];
		if(interim[idx] > dist)
			interim[idx] = dist;
	}
	
	for(int i=0;i<interim.size();i++) {
		dist = interim[i]+idtTarget[i];
		if(dist < minDist)
			minDist = dist;
	}
	interim.clear();
	
	return minDist;
}

pair<pair<int,int>,int> ParDiSP::tripleDistanceJoinBorders(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry, int srcComp, int trgComp) {
	
	pair<pair<int,int>,int> borders;
	
	borders.second = INT_MAX;
	int dist;
	int idx = -1;
	int modulo;
	
	vector<int> interim(idtTarget.size(),INT_MAX);
	vector<int> bordersCand(idtTarget.size());
	
	for(int i=0;i<cdmEntry.size();i++) {
		modulo = i%idtSource.size();
		if(modulo == 0) 
			idx++;
		dist = cdmEntry[i] + idtSource[modulo];
		if(interim[idx] > dist) {
			interim[idx] = dist;
			//bordersCand[idx] = this->outBordersStore[srcComp][modulo];
			bordersCand[idx] = modulo;
		}	
	}
	
	for(int i=0;i<interim.size();i++) {
		dist = interim[i]+idtTarget[i];
		if(dist < borders.second) {
			borders.second = dist;
			borders.first.first = bordersCand[i];
			//borders.first.second = this->incBordersStore[trgComp][i];
			borders.first.second = i;
		}
	}
	interim.clear();
	
	return borders;
}

Path ParDiSP::nodeToBorderRetrieval(int source, int sourceBorderIdx) {
    
    int activeComponent = this->cm[source];
    int target = this->outBordersStore[activeComponent][sourceBorderIdx];
        
    PriorityQueueAs Q;
    Path resPath, newPath;
    int newLength = 0,     resLength = 0;
    int newLowerBound;
    int bTo, bFrom;
    std::vector<bool> visited(rN->numNodes, false);
    vector<Label*> allCreatedLabels;
    vector<int> distances(this->rN->numNodes,INT_MAX);
        
    newLowerBound = this->outIDT[source][sourceBorderIdx];
    int totalDist = newLowerBound;
    newPath.push_back(source);
    
    Label *label = new Label(source, newPath, newLength,newLowerBound);
    Q.push(label);
    allCreatedLabels.push_back(label);
    
    while (!Q.empty())     {
        Label *curLabel = Q.top();
        Q.pop();
        distances[curLabel->node_id] = curLabel->length;
        
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
                if(!this->cem[iterAdj->first][activeComponent])
                	continue;
                
                if(this->cm[iterAdj->first] == activeComponent) {
                	newLowerBound = newLength + this->outIDT[iterAdj->first][sourceBorderIdx];
                }
                else {
                	newLowerBound = newLength;
                }
                
                if(newLowerBound > totalDist)
                	continue;
                
				//cout << "[DEBUG] " << newLowerBound << endl;
                if (!visited[iterAdj->first] || newLength < distances[iterAdj->first]) {
                	Label *tlabel = new Label(iterAdj->first, newPath, newLength, newLowerBound);
                    Q.push(tlabel);
                	allCreatedLabels.push_back(tlabel);
                }
            }
        }
    }
    //cout << "Direct retrieval labels = " << allCreatedLabels.size() << endl; 
    for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
    
    return resPath;
}


Path ParDiSP::borderToNodeRetrieval(int targetBorderIdx, int target) {
    
    int activeComponent = this->cm[target];
    int source = this->incBordersStore[activeComponent][targetBorderIdx];
        
    PriorityQueueAs Q;
    Path resPath, newPath;
    int newLength = 0,     resLength = 0;
    int newLowerBound;
    int bTo, bFrom;
    std::vector<bool> visited(rN->numNodes, false);
    vector<Label*> allCreatedLabels;
    vector<int> distances(this->rN->numNodes,INT_MAX);
        
    newLowerBound =  this->incIDT[target][targetBorderIdx];
    int totalDist = newLowerBound;
    newPath.push_back(target);
    
    Label *label = new Label(target, newPath, newLength,newLowerBound);
    Q.push(label);
    allCreatedLabels.push_back(label);
    
    while (!Q.empty())     {
        Label *curLabel = Q.top();
        Q.pop();
        distances[curLabel->node_id] = curLabel->length;
        
        if (visited[curLabel->node_id])
            continue;
        
        visited[curLabel->node_id] = true;
        
        // Found target.
        if (curLabel->node_id == source) {
            resPath = curLabel->path;
            resLength = curLabel->length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListInc[curLabel->node_id].begin(); iterAdj != rN->adjListInc[curLabel->node_id].end(); iterAdj++) {
                
                newLength = curLabel->length + iterAdj->second;
                newPath = curLabel->path;
                newPath.push_back(iterAdj->first);
                if(!this->cem[iterAdj->first][activeComponent])
                	continue;
                
                if(this->cm[iterAdj->first] == activeComponent) {
                	newLowerBound = newLength + this->incIDT[iterAdj->first][targetBorderIdx];
                }
                else {
                	newLowerBound = newLength;
                }
                
                if(newLowerBound > totalDist)
                	continue;
                
				//cout << "[DEBUG] " << newLowerBound << endl;
                if (!visited[iterAdj->first] || newLength < distances[iterAdj->first]) {
                	Label *tlabel = new Label(iterAdj->first, newPath, newLength, newLowerBound);
                    Q.push(tlabel);
                	allCreatedLabels.push_back(tlabel);
                }
            }
        }
    }
    //cout << "Direct retrieval labels = " << allCreatedLabels.size() << endl; 
    for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
    reverse(resPath.begin(), resPath.end());
    
    return resPath;
}