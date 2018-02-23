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