#include <iostream>
#include <chrono>
#include "Time.h"
using namespace std;
using namespace std::chrono;

void Time::czasStart() {
	czasPoczatkowy = high_resolution_clock::now();
}

void Time::czasStop() {
	czasKoncowy = high_resolution_clock::now();
}

long Time::czasWykonania() {

	return duration_cast<microseconds>(Time::czasKoncowy - Time::czasPoczatkowy).count();
}

long Time::czasWykonaniaMili() {

	return duration_cast<milliseconds>(Time::czasKoncowy - Time::czasPoczatkowy).count();
}

long Time::czasWykonaniaNano() {

		return duration_cast<nanoseconds>(Time::czasKoncowy - Time::czasPoczatkowy).count();

}

long Time::czasWykonaniaSek() {

	return duration_cast<seconds>(Time::czasKoncowy - Time::czasPoczatkowy).count();

}

Time::Time()
{
	
}

void Time::start()
{
	tstart = std::chrono::steady_clock::now();
}

void Time::stop()
{
	tstop = std::chrono::steady_clock::now();
	measurement = tstop - tstart;
}

double Time::read()
{
	return measurement.count();
}