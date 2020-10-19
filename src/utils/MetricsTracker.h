
#ifndef __METRICS_TRACKER
#define __METRICS_TRACKER

#include "../General.h"

template<class T>
class MetricsTracker
{
	
private:
	int window_size;
	string name;
	T latest_window_sum;
	list<T> window;
	vector<T> iteration_averages;

public:
	/* Constructor and setup methods */
	MetricsTracker(int window_size, string name);
	
	/* main loop */
	void addSample(T sample);
	vector<T>& getCalculatedAverages();
	string getName();

};

#endif


