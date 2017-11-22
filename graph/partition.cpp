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
 
 #include "partition.hpp"

pair<ComponentsMap,int> loadComponents(RoadNetwork *rN, string filename) {
	pair<ComponentsMap,int> map;
	map.first.reserve((rN->numNodes));
	
	ifstream graphIn(filename.c_str());
	
	if (!graphIn.is_open()) { 
	 	cerr << "Cannot open " << filename << endl; 
	   	exit(EXIT_FAILURE);
	}
	
	string line;
	int verticesNum = -1, edgesNum = -1;
	int max = -1;
	
	for(int i=0;i<rN->numNodes;i++) {
		getline(graphIn,line);
		
		int part;
		istringstream(line) >> part;
		map.first.push_back(part);
		if(part > max)
			max = part;
	}
	map.second = max+1;
	
	graphIn.close();
	return map;
}