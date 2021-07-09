#include "Timer.h"

std::chrono::steady_clock::time_point Timer::m_T0 = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::duration Timer::m_DeltaTime = std::chrono::high_resolution_clock::duration(0);
std::chrono::high_resolution_clock::duration Timer::m_TotalTime = std::chrono::high_resolution_clock::duration(0);

Timer::Timer()
{
}

void Timer::Tick()
{
	auto t1 = std::chrono::high_resolution_clock::now();
	m_DeltaTime = t1 - m_T0;
	m_TotalTime += m_DeltaTime;
	m_T0 = t1;
}

void Timer::Reset()
{
	m_T0 = std::chrono::high_resolution_clock::now();
	m_DeltaTime = std::chrono::high_resolution_clock::duration();
	m_TotalTime = std::chrono::high_resolution_clock::duration();
}