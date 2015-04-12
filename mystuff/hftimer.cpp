#include "hftimer.h"
//#include <iostream>


#ifdef LINUX
    
void hftimer::startTimer()
{
	clock_gettime(CLOCK_MONOTONIC, &tp1);
//	std::cout<<"tp1"<<tp1.tv_sec<<"s"<<tp1.tv_nsec<<std::endl;

}

int hftimer::getTime()
{
	clock_gettime(CLOCK_MONOTONIC, &tp2);

	int time = (tp2.tv_sec-tp1.tv_sec)*1000;
	int ms= (tp2.tv_nsec-tp1.tv_nsec) / 1000000;

	//std::cout<<"tp2"<<tp2.tv_sec<<"s"<<tp2.tv_nsec<<std::endl;
	//std::cout<<"time"<<time<<"s"<<ms<<std::endl;

	return ms<0?time+1000+ms:time+ms;
}
#endif

#ifdef WIN32

void hftimer::startTimer()
{
	QueryPerformanceCounter(&tp1);
}

int hftimer::getTime()
{
	// (tp2.QuadPart-tp1.QuadPart)/res.QuadPart is elapsed time in seconds
	QueryPerformanceCounter(&tp2);
	LONGLONG time = 1000*(tp2.QuadPart-tp1.QuadPart)/res.QuadPart;

	return (int)time; // returns elapsed time in miliseconds
}

#endif
