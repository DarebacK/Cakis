#include "stdafx.h"
#include "TimeInfoDisplayer.h"
#include "UpdateInfo.h"
#include "DrawInfo.h"
#include "Timing/Clock.h"

using namespace DE;
using namespace DE::Timing;

const size_t Demo::TimeInfoDisplayer::m_defaultFrameTimesSize{ 60 };

Demo::TimeInfoDisplayer::TimeInfoDisplayer()
{
	m_fpsText.Position = { 0.0f, 0.0f };
	m_frameTimeText.Position = { 0.0f, 20.0f };
	m_timeElapsedText.Position = { 0.0f, 40.0f };	
}

void Demo::TimeInfoDisplayer::OnUpdate(const UpdateInfo& info)
{
	if(m_iterator >= m_defaultFrameTimesSize)
	{
		m_iterator = 0;
	}

	auto frameTime = GetDeltaTime(info.Clock);
	m_frameTimeText.Text = L"Frame time: " + std::to_wstring(frameTime) + L" s";
	
	m_frameTimes[m_iterator++] = frameTime;
	int averageFps = static_cast<int>(m_frameTimes.size() / std::accumulate(m_frameTimes.cbegin(), m_frameTimes.cend(), 0.0));
	m_fpsText.Text = L"Fps: " + std::to_wstring(averageFps);

	auto timeElapsed = GetElapsedTime<std::ratio<1>, double>(info.Clock);
	m_timeElapsedText.Text = L"Time elapsed: " + std::to_wstring(timeElapsed) + L" s";
}

void Demo::TimeInfoDisplayer::OnDraw(const DrawInfo& info)
{
	info.SpriteTextDrawer.EnqueueDrawCall(SpriteFontName::SegoeUI_14, m_timeElapsedText);
	info.SpriteTextDrawer.EnqueueDrawCall(SpriteFontName::SegoeUI_14, m_frameTimeText);
	info.SpriteTextDrawer.EnqueueDrawCall(SpriteFontName::SegoeUI_14, m_fpsText);
}
