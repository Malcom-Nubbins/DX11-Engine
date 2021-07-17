#include "Timer.h"

Timer::Timer() 
	: m_T0(std::chrono::steady_clock::now())
	, m_DeltaTime(std::chrono::steady_clock::duration(0))
	, m_TotalTime(std::chrono::steady_clock::duration(0))
{
}

void Timer::Tick()
{
	auto t1 = std::chrono::steady_clock::now();
	m_DeltaTime = t1 - m_T0;
	m_TotalTime += m_DeltaTime;
	m_T0 = t1;
}

void Timer::Reset()
{
	m_T0 = std::chrono::steady_clock::now();
	m_DeltaTime = std::chrono::steady_clock::duration();
	m_TotalTime = std::chrono::steady_clock::duration();
}