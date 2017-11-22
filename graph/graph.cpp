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
 
 #include "graph.hpp"

RoadNetwork::RoadNetwork(const char *filename) {
    
    FILE *fp;
    int lnode, rnode, tmp;
    int w;
    char c;

    fp = fopen(filename, "r");
    fscanf(fp, "%c\n", &c);
    fscanf(fp, "%d %d\n", &this->numNodes, &this->numEdges);
    this->adjListOut = new EdgeList[this->numNodes];
    this->adjListInc = new EdgeList[this->numNodes];
    
    while (fscanf(fp, "%d %d %d %d\n", &lnode, &rnode, &w, &tmp) != EOF) {
        this->adjListOut[lnode].insert(make_pair(rnode, w));
        this->adjListInc[rnode].insert(make_pair(lnode, w));
    }
    fclose(fp);
    
    #ifdef VERIFY_GRAPH
    	cout << "Graph verification required!" << endl;
    #endif
}

int RoadNetwork::getEdgeWeight(int lnode, int rnode) {
    return this->adjListOut[lnode][rnode];
}

bool addEdge(int src, int trg, int w) {
	// if contains return false
	return true;
}

bool removeEdge(int src, int trg) {
	// if edge does not exist return false
	return false;
}

void RoadNetwork::print() {
    for (int nid = 0; nid < this->numNodes; nid++) {
        for (EdgeList::iterator iterAdj = this->adjListOut[nid].begin(); iterAdj != this->adjListOut[nid].end(); iterAdj++)
            cout << nid << " " << iterAdj->first << " " << iterAdj->second << endl;
    }
}

set<int> RoadNetwork::neighborsOf(int node_id) {
	set<int> neighbors;
	for (EdgeList::iterator iterAdj = this->adjListOut[node_id].begin(); iterAdj != this->adjListOut[node_id].end(); iterAdj++)
        neighbors.insert(iterAdj->first);
	for (EdgeList::iterator iterAdj = this->adjListInc[node_id].begin(); iterAdj != this->adjListInc[node_id].end(); iterAdj++)
        neighbors.insert(iterAdj->first);
	return neighbors;
}

RoadNetwork::~RoadNetwork() {
    delete[] this->adjListOut;
    delete[] this->adjListInc;
}
