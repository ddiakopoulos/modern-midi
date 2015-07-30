#ifndef GRAPHENE_QPC_TIMER_H
#define GRAPHENE_QPC_TIMER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

class QPCTimer
{
	LARGE_INTEGER start_timestamp;
	LARGE_INTEGER stop_timestamp;
	LARGE_INTEGER timer_frequency;

public:

	QPCTimer()
	{
		start_timestamp.QuadPart = 0;
		stop_timestamp.QuadPart = 0;
	}

	void start()
	{
		QueryPerformanceFrequency(&timer_frequency);
		QueryPerformanceCounter(&start_timestamp);
	}

	void stop()
	{
		QueryPerformanceCounter(&stop_timestamp);
	}

	double running_diff_ms() const
	{
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		return (double) (tmp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart * 1000;
	}

	double running_diff_seconds() const
	{
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		return (double) (tmp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart;
	}

	double diff_ms() const
	{
		return (double)(stop_timestamp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart * 1000;
	}
};


#endif
