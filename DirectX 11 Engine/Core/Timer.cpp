#include "Timer.h"


Timer::Timer() : _secondsPerCount(0.0), _deltaTime(-1.0),  _baseTime(0), _pausedTime(0), _prevTime(0), _currTime(0), _paused(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	_secondsPerCount = 1.0f / (double)countsPerSec;
}


Timer::~Timer()
{
}

float Timer::GameTime() const
{
	if (_paused)
	{
		return (float)(((_stopTime - _pausedTime) - _baseTime)*_secondsPerCount);
	}
	else
	{
		return (float)(((_currTime - _pausedTime) - _baseTime)*_secondsPerCount);
	}
}

float Timer::DeltaTime() const
{
	return (float)_deltaTime;
}

void Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	_baseTime = currTime;
	_prevTime = currTime;
	_stopTime = 0;
	_paused = false;
}

void Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (_paused)
	{
		_pausedTime += (startTime - _stopTime);

		_prevTime = startTime;

		_stopTime = 0;
		_paused = false;
	}
}

void Timer::Stop()
{
	if (!_paused)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		_stopTime = currTime;
		_paused = true;
	}
}

void Timer::Tick()
{
	if (_paused)
	{
		_deltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	_currTime = currTime;

	_deltaTime = (_currTime - _prevTime)*_secondsPerCount;
	_prevTime = _currTime;

	if (_deltaTime < 0.0)
		_deltaTime = 0.0;
}
