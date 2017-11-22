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