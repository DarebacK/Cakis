#include "stdafx.h"
#include "Clock.h"


DE::Timing::Clock::Clock()
{
	Reset();
}

void DE::Timing::Clock::Update()
{
	auto timeTemp = ClockType::now();
	m_deltaTime = (timeTemp - m_actualTime).count();
	m_actualTime = timeTemp;
}

void DE::Timing::Clock::Reset()
{
	m_startTime = ClockType::now();
	m_actualTime = m_startTime;
	m_deltaTime = 0.0;
}