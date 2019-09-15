#pragma once
#include "Globals\stdafx.h"
class Timer
{
public:
	Timer();
	~Timer();

	float GameTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

	bool IsScenePaused() const { return _paused; }

private:
	double _secondsPerCount;
	double _deltaTime;

	__int64 _baseTime;
	__int64 _pausedTime;
	__int64 _stopTime;
	__int64 _prevTime;
	__int64 _currTime;

	bool _paused;
};

