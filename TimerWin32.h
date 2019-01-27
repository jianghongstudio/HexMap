#pragma once
#include "Timer.h"

class TimerWin32 : public Timer
{
public:
	TimerWin32();
	virtual ~TimerWin32();

	virtual void Reset() override; // Call before message loop.
	virtual void Start() override; // Call when unpaused.
	virtual void Stop() override;  // Call when paused.
	virtual void Tick() override;  // Call every frame.

};



