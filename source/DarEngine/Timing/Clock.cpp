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

DE::Timing::Clock::Clock()
{
	m_frequency = QueryFrequency();
	Reset();
}

void DE::Timing::Clock::Update()
{
	auto timeTemp = QueryTime() / m_frequency;
	m_deltaTime = (timeTemp - m_actualTime);
	m_actualTime = timeTemp;
}

void DE::Timing::Clock::Reset()
{
	m_startTime = QueryTime() / m_frequency;
	m_actualTime = m_startTime;
	m_deltaTime = 0;
}