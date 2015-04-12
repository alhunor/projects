#ifndef _HFTIMER_H_
#define _HFTIMER_H_

#ifdef LINUX
#include <time.h> //must be linked againt librt
class hftimer
{
	public: 
		hftimer() {	clock_getres(CLOCK_MONOTONIC, &res);}
		void startTimer();
		int getTime();// returns time in ms since last call to startTimer()
protected:
	struct timespec res;
	struct timespec tp1, tp2;
};
#endif

#ifdef WIN32
#include <windows.h>
class hftimer
{
	public: 
		hftimer() {	QueryPerformanceFrequency(&res);}
		void startTimer();
		int getTime();// returns time in ms since last call to startTimer()
protected:
	LARGE_INTEGER res;
	LARGE_INTEGER tp1, tp2;
};
#endif

#endif
