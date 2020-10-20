
#include "MetricsTracker.h"

template<class T>
MetricsTracker<T>::MetricsTracker( int _window_size, string _name) : window_size(_window_size), name(_name){}

template<class T>
void MetricsTracker<T>::addSample(T sample)
{
	if(window_size <= 0){

		iteration_averages.push_back(sample);
	
	}else{

		window.push_back(sample);
		int current_size = window.size();
		if(current_size == window_size){
			
			latest_window_sum = std::accumulate(window.begin(), window.end(), 0);
			iteration_averages.push_back(latest_window_sum/window_size);
		
		} else if(current_size > window_size){

			latest_window_sum = latest_window_sum - window.front() + window.back();
			iteration_averages.push_back(latest_window_sum/window_size);
			window.pop_front();

		} else {
				
			//No enough data to calculate a moving average
			iteration_averages.push_back(0);

		}

	}
}

template<class T>
vector<T>& MetricsTracker<T>::getCalculatedAverages(){	
	return iteration_averages;
}

template<class T>
string MetricsTracker<T>::getName(){
	return name;
}

