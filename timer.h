#pragma once
#include <iostream>
#include <windows.h>
#undef CreateWindow // seriously Microsoft, sort yourselves out.
namespace SOF
{
	class Timer
	{
		LARGE_INTEGER timer;
		float inverseTimerFreq;
		LARGE_INTEGER TimeNow() const
		{
			LARGE_INTEGER val;
			QueryPerformanceCounter(&val);
			return val;
		}
	public:
		Timer()
		{
			LARGE_INTEGER val;
			QueryPerformanceFrequency(&val);
			inverseTimerFreq = 1.0f / (float)val.QuadPart;
		}
		void Reset()
		{
			timer = TimeNow();
		}
		float Elapsed() const
		{
			long long int elapsed = TimeNow().QuadPart - timer.QuadPart;
			return (float)elapsed * inverseTimerFreq;
		}
	};
}