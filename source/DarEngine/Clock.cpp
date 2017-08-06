#include "stdafx.h"
#include "Clock.h"
#include "Diagnostics/Exception.h"

namespace
{
	LONGLONG QueryFrequency()
	{
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency))
		{
			throw DE::Diagnostics::Exception{ L"QueryPerformanceFrequency() failed." };
		}
		return frequency.QuadPart;
	}

	LONGLONG QueryTime()
	{
		LARGE_INTEGER time;
		if(!QueryPerformanceCounter(&time))
		{
			throw DE::Diagnostics::Exception{ L"QueryPerformanceCounter() failed." };
		}
		return time.QuadPart;
	}
}

DE::Clock::Clock()
{
	m_frequency = QueryFrequency();
	Reset();
}

void DE::Clock::Update()
{
	m_lastTime = m_actualTime;
	m_actualTime = QueryTime();
}

void DE::Clock::Reset()
{
	m_startTime = QueryTime();
	m_actualTime = m_startTime;
	m_lastTime = m_startTime;
}
