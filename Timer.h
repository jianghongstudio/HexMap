#pragma once
class Timer
{
public:
	Timer();
	virtual ~Timer();

	float TotalTime()const;  // in seconds
	float DeltaTime()const; // in seconds

	virtual void Reset() = 0; // Call before message loop.
	virtual void Start() = 0; // Call when unpaused.
	virtual void Stop() = 0;  // Call when paused.
	virtual void Tick() = 0;  // Call every frame.

protected:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_Stopped;
};

