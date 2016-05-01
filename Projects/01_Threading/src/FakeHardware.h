#ifndef _FAKE_HARDWARE_H_
#define _FAKE_HARDWARE_H_

#include <thread>

//Simulation of hardware that takes some amount of time to return a result

class FakeHardware
{
public:
	FakeHardware(int minTimeMS, int maxTimeMS) : 
		m_minTime(minTimeMS), m_maxTime(maxTimeMS)
	{}
	~FakeHardware() {}

	int GetSomeResult()
	{
		int randNum = glm::linearRand(m_minTime, m_maxTime);
		std::this_thread::sleep_for(std::chrono::milliseconds(randNum));

		//return a 'result'
		return randNum;
	}

private:
	int m_minTime;
	int m_maxTime;
};

#endif