#include "stdafx.h"
#include "TimeInfoDisplayer.h"
#include "UpdateInfo.h"
#include "DrawInfo.h"
#include "Timing/Clock.h"
#include <iomanip>

using namespace DE;
using namespace DE::Timing;

const size_t Demo::TimeInfoDisplayer::m_defaultFrameTimesSize{ 60 };

Demo::TimeInfoDisplayer::TimeInfoDisplayer()
{
	m_text.Position = { 0.0f, 0.0f };
}

void Demo::TimeInfoDisplayer::OnUpdate(const UpdateInfo& info)
{
	if(m_iterator >= m_defaultFrameTimesSize)
	{
		m_iterator = 0;
	}

	auto frameTime = GetDeltaTime(info.HighResolutionClock);
	m_frameTimes[m_iterator++] = frameTime;
	int averageFps = static_cast<int>(m_frameTimes.size() / std::accumulate(m_frameTimes.cbegin(), m_frameTimes.cend(), 0.0));
	auto timeElapsed = GetElapsedTime<std::ratio<1>, double>(info.HighResolutionClock);
	std::tm currentLocalTime = TimePointToLocalTime(info.SystemClock.GetCurrentTimePoint());

	m_text.Text = L"Frame time: " + std::to_wstring(frameTime) + L" s\n" +
		L"Fps: " + std::to_wstring(averageFps) + L"\n" +
		L"Time elapsed: " + std::to_wstring(timeElapsed) + L" s\n" +
		L"Current local time: " + TimeToWString(currentLocalTime);

	
	
	//TODO: display current local time

}

void Demo::TimeInfoDisplayer::OnDraw(const DrawInfo& info)
{
	info.SpriteTextDrawer.EnqueueDrawCall(SpriteFontName::SegoeUI_14, m_text);
}
