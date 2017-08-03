#include "stdafx.h"
#include "Clock.h"
#include "Exception.h"

namespace
{
	LONGLONG QueryFrequency()
	{
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency))
		{
			throw DarEngine::Exception{ L"QueryPerformanceFrequency() failed." };
		}
		return frequency.QuadPart;
	}

	LONGLONG QueryTime()
	{
		LARGE_INTEGER time;
		if(!QueryPerformanceCounter(&time))
		{
			throw DarEngine::Exception{ L"QueryPerformanceCounter() failed." };
		}
		return time.QuadPart;
	}
}

DarEngine::Clock::Clock()
{
	m_frequency = QueryFrequency();

	Reset();
}

LONGLONG DarEngine::Clock::GetStartTime() const
{
	return m_startTime;
}

LONGLONG DarEngine::Clock::GetActualTime() const
{
	return m_actualTime;
}

LONGLONG DarEngine::Clock::GetLastTime() const
{
	return m_lastTime;
}

LONGLONG DarEngine::Clock::GetFrequency() const
{
	return m_frequency;
}

void DarEngine::Clock::Update()
{
	m_lastTime = m_actualTime;
	m_actualTime = QueryTime();
}

void DarEngine::Clock::Reset()
{
	m_startTime = QueryTime();
	m_actualTime = m_startTime;
	m_lastTime = m_startTime;
}
