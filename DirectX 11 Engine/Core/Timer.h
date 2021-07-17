#pragma once
#include <chrono>
class Timer
{
public:
	Timer();

	void Tick();
	void Reset();

	double GetDeltaNanoseconds() { return m_DeltaTime.count() * 1.0; }
	double GetDeltaMicroseconds() { return m_DeltaTime.count() * 1e-3; }
	double GetDeltaMilliseconds() { return m_DeltaTime.count() * 1e-6; }
	double GetDeltaSeconds() { return m_DeltaTime.count() * 1e-9; }

	double GetTotalNanoseconds() { return m_TotalTime.count() * 1.0; }
	double GetTotalMicroseconds() { return m_TotalTime.count() * 1e-3; }
	double GetTotalMilliseconds() { return m_TotalTime.count() * 1e-6; }
	double GetTotalSeconds() { return m_TotalTime.count() * 1e-9; }

private:
	std::chrono::steady_clock::time_point m_T0;

	std::chrono::steady_clock::duration m_DeltaTime;
	std::chrono::steady_clock::duration m_TotalTime;
};

