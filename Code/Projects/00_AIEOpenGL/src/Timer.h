#ifndef _TIMER_H_
#define _TIMER_H_

#include <Windows.h>

class Timer
{
public:
	Timer() 
	{	
		//query the speed of the CPU clock cycles 
		QueryPerformanceFrequency(&m_perfFreq);
	}

	void Start() { QueryPerformanceCounter(&m_startTime); }

	void End() { QueryPerformanceCounter(&m_endTime); }

	double GetTimeDiff() const { return (m_endTime.QuadPart - m_startTime.QuadPart) / (double)m_perfFreq.QuadPart; }

private:
	LARGE_INTEGER m_perfFreq;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_endTime;
};

#endif