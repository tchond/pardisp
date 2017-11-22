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
 
 #ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <vector>
#include <fstream> 
#include <sstream>

#include <boost/algorithm/string.hpp>

using namespace std;

double GetTimeSec(void);
double GetTimeMicros(void);

typedef pair<int,int> Query;

pair<vector<Query>,vector<int>> loadQueries(string);

class TimingCPU {
	clock_t start;
public:
	TimingCPU();
	virtual ~TimingCPU();

	void StartTimer();
	clock_t StopTimer();
};

class TimingPhysical {
	timeval ts, te;
public:
	TimingPhysical();
	virtual ~TimingPhysical();

	void StartTimer();
	double StopTimer();
};
#endif 