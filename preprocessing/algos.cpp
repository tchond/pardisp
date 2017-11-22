#include "algos.hpp"

pair<Path, double> Dijkstra(RoadNetwork *rN, int source, int target) {
    
    PriorityQueue Q;
    Path resPath, newPath;
    double newLength = 0,     resLength = 0;
    std::vector<bool> visited(rN->numNodes, false);
    
    newPath.push_back(source);
    Q.push(Label(source, newPath, newLength));
    while (!Q.empty())     {
        Label curLabel = Q.top();
        Q.pop();
        
        if (visited[curLabel.node_id])
            continue;
        
        visited[curLabel.node_id] = true;
        
        // Found target.
        if (curLabel.node_id == target) {
            resPath = curLabel.path;
            resLength = curLabel.length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel.node_id].begin(); iterAdj != rN->adjListOut[curLabel.node_id].end(); iterAdj++) {
                newLength = curLabel.length + iterAdj->second;
                newPath = curLabel.path;
                newPath.push_back(iterAdj->first);
                if (!visited[iterAdj->first])
                    Q.push(Label(iterAdj->first, newPath, newLength));
            }
        }
    }
    
    int visitedCount = 0;
    for(int i=0;i<visited.size();i++) {
    	if(visited[i])
    		visitedCount++;
    }
    //cout << "Dijkstra visited = " << visitedCount << endl; 
    
    return make_pair(resPath, resLength);
}

/*
	This method is used to compute the extended components.
*/

set<int> getPathsUnion(RoadNetwork *rN, unordered_set<int> &sources, unordered_set<int> &targets) {
	set<int> nodesUnion;
	
	for (unordered_set<int>::iterator it = sources.begin(); it != sources.end(); it++) {
		int source = *it;
		int resultSize = 0;
		PriorityQueue Q;
   		Path resPath, newPath;
    	double newLength = 0, resLength = 0;
    	std::vector<bool> visited(rN->numNodes, false);
    
    	newPath.push_back(source);
    	Q.push(Label(source, newPath, newLength));
    	while (!Q.empty())     {
        	Label curLabel = Q.top();
        	Q.pop();
        	
        	if (visited[curLabel.node_id])
            	continue;
      
        	if (targets.find(curLabel.node_id) != targets.end()) {
        		for(int i=0;i<curLabel.path.size();i++)
            		nodesUnion.insert(curLabel.path[i]);
            	resLength = curLabel.length;
            	resultSize++;
            	if(resultSize == targets.size())
            		break;
        	}
        
        	visited[curLabel.node_id] = true;
    
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel.node_id].begin(); iterAdj != rN->adjListOut[curLabel.node_id].end(); iterAdj++) {
               	newLength = curLabel.length + iterAdj->second;
               	newPath = curLabel.path;
               	newPath.push_back(iterAdj->first);
               	if (!visited[iterAdj->first])
                   	Q.push(Label(iterAdj->first, newPath, newLength));
            }
    	}
    	
	}
	
	return nodesUnion;
}

int tripleDistanceJoin(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry) {
	
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

pair<int,int> tripleDistanceJoinBorders(vector<int> &idtSource, vector<int> &idtTarget,  vector<int> &cdmEntry) {
	
	pair<int,int> borders;
	
	int minDist = INT_MAX;
	int dist;
	int idx = -1;
	int modulo;
	
	vector<int> interim(idtTarget.size(),INT_MAX);

	for(int i=0;i<cdmEntry.size();i++) {
		modulo = i%idtSource.size();
		if(modulo == 0) 
			idx++;
		dist = cdmEntry[i] + idtSource[modulo];
		if(interim[idx] > dist) {
			interim[idx] = dist;
			borders.first = modulo;
		}	
	}
	
	for(int i=0;i<interim.size();i++) {
		dist = interim[i]+idtTarget[i];
		if(dist < minDist) {
			minDist = dist;
			borders.second = i;
		}
	}
	interim.clear();
	
	return borders;
}

pair<Path, double> ALT(RoadNetwork *rN, int source, int target, vector<int> &fromLandmark, vector<int> &toLandmark) {
    
    PriorityQueueAs Q;
    Path resPath, newPath;
    int newLength = 0,     resLength = 0;
    int newLowerBound;
    int bTo, bFrom;
    std::vector<bool> visited(rN->numNodes, false);
    
    bTo = abs(toLandmark[source]-toLandmark[target]);
    bFrom = abs(fromLandmark[target]-fromLandmark[source]);
    if(bTo > bFrom)
    	newLowerBound = newLength + bTo;
    else 
    	newLowerBound = newLength + bFrom;
    
    //newLowerBound = newLength + abs(toLandmark[source]-fromLandmark[target]);
    
    newPath.push_back(source);
    Q.push(Label(source, newPath, newLength,newLowerBound));
    while (!Q.empty())     {
        Label curLabel = Q.top();
        Q.pop();
        
        if (visited[curLabel.node_id])
            continue;
        
        visited[curLabel.node_id] = true;
        
        // Found target.
        if (curLabel.node_id == target) {
            resPath = curLabel.path;
            resLength = curLabel.length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel.node_id].begin(); iterAdj != rN->adjListOut[curLabel.node_id].end(); iterAdj++) {
                newLength = curLabel.length + iterAdj->second;
                newPath = curLabel.path;
                newPath.push_back(iterAdj->first);
                
               	bTo = abs(toLandmark[source]-toLandmark[target]);
    			bFrom = abs(fromLandmark[target]-fromLandmark[source]);
    			if(bTo > bFrom)
    				newLowerBound = newLength + bTo;
    			else 
    				newLowerBound = newLength + bFrom;
    			
    			//newLowerBound = newLength + abs(toLandmark[source]-toLandmark[target]);
    				
                if (!visited[iterAdj->first])
                    Q.push(Label(iterAdj->first, newPath, newLength, newLowerBound));
            }
        }
    }
    
    int visitedCount = 0;
    for(int i=0;i<visited.size();i++) {
    	if(visited[i])
    		visitedCount++;
    }
    cout << "ALT visited = " << visitedCount << endl; 
    
    return make_pair(resPath, resLength);
}

pair<Path, double> DijkstraLimited(RoadNetwork *rN, int source, int target, vector<int> &cm, vector<vector<bool>> &cem) {
    
    PriorityQueue Q;
    Path resPath, newPath;
    double newLength = 0,     resLength = 0;
    std::vector<bool> visited(rN->numNodes, false);
    
    newPath.push_back(source);
    Q.push(Label(source, newPath, newLength));
    while (!Q.empty())     {
        Label curLabel = Q.top();
        Q.pop();
        
        if (visited[curLabel.node_id])
            continue;
        
        visited[curLabel.node_id] = true;
        
        // Found target.
        if (curLabel.node_id == target) {
            resPath = curLabel.path;
            resLength = curLabel.length;
            break;
        }
        // Expand search.
        else {
            // For each outgoing edge.
            for (EdgeList::iterator iterAdj = rN->adjListOut[curLabel.node_id].begin(); iterAdj != rN->adjListOut[curLabel.node_id].end(); iterAdj++) {
                newLength = curLabel.length + iterAdj->second;
                newPath = curLabel.path;
                newPath.push_back(iterAdj->first);
                //if (!visited[iterAdj->first])
                if (!visited[iterAdj->first] && (cm[iterAdj->first] == cm[source] || cem[iterAdj->first][cm[source]]))
                    Q.push(Label(iterAdj->first, newPath, newLength));
            }
        }
    }
    
    int visitedCount = 0;
    for(int i=0;i<visited.size();i++) {
    	if(visited[i])
    		visitedCount++;
    }
    //cout << "Dijkstra visited = " << visitedCount << endl; 
    
    return make_pair(resPath, resLength);
}
