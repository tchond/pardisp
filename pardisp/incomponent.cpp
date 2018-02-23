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

double ParDiSP::ALT(RoadNetwork *rN, int source, int target) {
    
    PriorityQueueAs Q;
    int newLength = 0, resLength = 0;
    int newLowerBound;
    int bTo, bFrom;
    std::vector<bool> visited(rN->numNodes, false);
    vector<Label*> allCreatedLabels;
        
	int landmark = 0;
    newLowerBound = 0;
    int tmpBound;
    for(int l = 0;l<this->outIDT[source].size();l++) {
    	bTo = abs(this->outIDT[source][l]-this->outIDT[target][l]);
   		bFrom = abs(this->incIDT[target][l]-this->incIDT[target][l]);
    	if(bTo > bFrom)
    		tmpBound = newLength + bTo;
    	else 
    		tmpBound = newLength + bFrom;
    	
    	if(tmpBound > newLowerBound) {
    		landmark = l;
    		newLowerBound = tmpBound;
    	}
    }    
         
    Label *label = new Label(source, newLength,newLowerBound);
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
            resLength = curLabel->length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel->node_id].begin(); iterAdj != rN->adjListOut[curLabel->node_id].end(); iterAdj++) {
                newLength = curLabel->length + iterAdj->second;
                
                if(this->cm[iterAdj->first] != this->cm[target] && this->cem[iterAdj->first][this->cm[target]]) {
                	newLowerBound = newLength;
                }
                else {
                	bTo = abs(this->outIDT[iterAdj->first][landmark]-this->outIDT[target][landmark]);
    				bFrom = abs(this->incIDT[iterAdj->first][landmark]-this->incIDT[target][landmark]);
    				if(bTo > bFrom)
    					newLowerBound = newLength + bTo;
    				else 
    					newLowerBound = newLength + bFrom;
                }
    				
                if (!visited[iterAdj->first]) {
                	Label *tlabel = new Label(iterAdj->first, newLength, newLowerBound);
                    Q.push(tlabel);
                	allCreatedLabels.push_back(tlabel);
                }
            }
        }
    }
    
    //cout << "ALT labels = " << allCreatedLabels.size() << endl; 
    for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
    
    return resLength;
}

pair<Path, double> ParDiSP::ALT_Path(RoadNetwork *rN, int source, int target) {
    
    PriorityQueueAs Q;
    Path resPath, newPath;
    int newLength = 0,     resLength = 0;
    int newLowerBound;
    int bTo, bFrom;
    std::vector<bool> visited(rN->numNodes, false);
    vector<Label*> allCreatedLabels;
        
	int landmark = 0;
    newLowerBound = 0;
    int tmpBound;
    for(int l = 0;l<this->outIDT[source].size();l++) {
    	bTo = abs(this->outIDT[source][l]-this->outIDT[target][l]);
   		bFrom = abs(this->incIDT[target][l]-this->incIDT[target][l]);
    	if(bTo > bFrom)
    		tmpBound = newLength + bTo;
    	else 
    		tmpBound = newLength + bFrom;
    	
    	if(tmpBound > newLowerBound) {
    		landmark = l;
    		newLowerBound = tmpBound;
    	}
    }    
     
    newPath.push_back(source);
    
    Label *label = new Label(source, newPath, newLength,newLowerBound);
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
                
                if(this->cm[iterAdj->first] != this->cm[target] && this->cem[iterAdj->first][this->cm[target]]) {
                	newLowerBound = newLength;
                }
                else {
                	bTo = abs(this->outIDT[iterAdj->first][landmark]-this->outIDT[target][landmark]);
    				bFrom = abs(this->incIDT[iterAdj->first][landmark]-this->incIDT[target][landmark]);
    				if(bTo > bFrom)
    					newLowerBound = newLength + bTo;
    				else 
    					newLowerBound = newLength + bFrom;
                }
    				
                if (!visited[iterAdj->first]) {
                	Label *tlabel = new Label(iterAdj->first, newPath, newLength, newLowerBound);
                    Q.push(tlabel);
                	allCreatedLabels.push_back(tlabel);
                }
            }
        }
    }
    
    //cout << "ALT labels = " << allCreatedLabels.size() << endl; 
    for(int i=0;i<allCreatedLabels.size();i++) {
    	delete allCreatedLabels[i];
    }
    allCreatedLabels.clear();
    
    return make_pair(resPath, resLength);
}