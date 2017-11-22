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
 
 #include "benchmark.hpp"

double GetTimeSec(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

double GetTimeMicros(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec/1e-6 + tv.tv_usec;
}

TimingCPU::TimingCPU() {
}


TimingCPU::~TimingCPU() {
}


void TimingCPU::StartTimer() {
	start = clock();
}

clock_t TimingCPU::StopTimer() {
	return clock()-start;
}

TimingPhysical::TimingPhysical() {
}

TimingPhysical::~TimingPhysical() {
}

void TimingPhysical::StartTimer() {
	gettimeofday(&this->ts, NULL);
}

double TimingPhysical::StopTimer() {
	gettimeofday(&this->te, NULL);
	return (te.tv_sec-ts.tv_sec) * 1000.0 + (te.tv_usec-ts.tv_usec) / 1000.0;	// In milliseconds
}

pair<vector<Query>,vector<int>> loadQueries(string filename) { 

	std::vector<Query> queries;
	vector<int> distances;
	int qNum;

	ifstream graphIn(filename.c_str());
	
	if (!graphIn.is_open()) { 
	 	cerr << "Cannot open " << filename << endl; 
	   	exit(EXIT_FAILURE);
	}
	
	string line;
	getline(graphIn,line);
	std::vector<std::string> strs;
	boost::split(strs, line, boost::is_any_of("\t "));
	
	istringstream(strs[0]) >> qNum;
	
	for(int i=0;i<qNum;i++) {
		getline(graphIn,line);
		int src,trg;
		int dist;
		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of("\t "));	
		
		istringstream(strs[0] ) >> src;
		istringstream(strs[1] ) >> trg;
		queries.push_back(make_pair(src,trg));
		distances.push_back(dist);
	}
	
	graphIn.close();

	return make_pair(queries,distances);
}

