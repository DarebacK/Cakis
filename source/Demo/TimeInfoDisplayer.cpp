#include "stdafx.h"
#include "TimeInfoDisplayer.h"
#include "UpdateInfo.h"
#include "DrawInfo.h"
#include "Timing/Clock.h"
#include <iomanip>

using namespace std;
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
	int averageFps = static_cast<int>(m_frameTimes.size() / accumulate(m_frameTimes.cbegin(), m_frameTimes.cend(), 0.0));
	auto timeElapsed = GetElapsedTime<ratio<1>, double>(info.HighResolutionClock);
	std::tm localTime = to_LocalTime(info.SystemClock.GetCurrentTimePoint());

	m_text.Text = L"Frame time: " + to_wstring(frameTime) + L" s\n" +
		L"Fps: " + to_wstring(averageFps) + L"\n" +
		L"Time elapsed: " + to_wstring(timeElapsed) + L" s\n" +
		L"Local time: " + to_wstring(localTime, L"%H:%M:%S");
}

void Demo::TimeInfoDisplayer::OnDraw(const DrawInfo& info)
{
	info.SpriteTextDrawer.EnqueueDrawCall(SpriteFontName::SegoeUI_14, m_text);
}
