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
			throw Dar::Exception{ L"QueryPerformanceFrequency() failed." };
		}
		return frequency.QuadPart;
	}

	LONGLONG QueryTime()
	{
		LARGE_INTEGER time;
		if(!QueryPerformanceCounter(&time))
		{
			throw Dar::Exception{ L"QueryPerformanceCounter() failed." };
		}
		return time.QuadPart;
	}
}

Dar::Clock::Clock()
{
	m_frequency = QueryFrequency();
	Reset();
}

void Dar::Clock::Update()
{
	m_lastTime = m_actualTime;
	m_actualTime = QueryTime();
}

void Dar::Clock::Reset()
{
	m_startTime = QueryTime();
	m_actualTime = m_startTime;
	m_lastTime = m_startTime;
}
