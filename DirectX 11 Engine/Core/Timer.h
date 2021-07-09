#pragma once
#include <chrono>
class Timer
{
public:
	Timer();

	static void Tick();
	void Reset();

	static double GetDeltaNanoseconds() { return m_DeltaTime.count() * 1.0; }
	static double GetDeltaMicroseconds() { return m_DeltaTime.count() * 1e-3; }
	static double GetDeltaMilliseconds() { return m_DeltaTime.count() * 1e-6; }
	static double GetDeltaSeconds() { return m_DeltaTime.count() * 1e-9; }

	static double GetTotalNanoseconds() { return m_TotalTime.count() * 1.0; }
	static double GetTotalMicroseconds() { return m_TotalTime.count() * 1e-3; }
	static double GetTotalMilliseconds() { return m_TotalTime.count() * 1e-6; }
	static double GetTotalSeconds() { return m_TotalTime.count() * 1e-9; }

private:
	static std::chrono::high_resolution_clock::time_point m_T0;

	static std::chrono::high_resolution_clock::duration m_DeltaTime;
	static std::chrono::high_resolution_clock::duration m_TotalTime;
};

