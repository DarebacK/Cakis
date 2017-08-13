#include "stdafx.h"
#include "TimeInfoDisplayer.h"
#include "UpdateInfo.h"
#include "DrawInfo.h"
#include "Timing/Clock.h"

const size_t Demo::TimeInfoDisplayer::m_defaultFrameTimesSize{ 60 };

Demo::TimeInfoDisplayer::TimeInfoDisplayer()
{
	m_timeElapsedText.Position = { 10.0f, 10.0f };
	m_frameTimeText.Position = { 10.0f, 30.0f };
	m_fpsText.Position = { 10.0f, 50.0f };
}

void Demo::TimeInfoDisplayer::OnUpdate(const DE::UpdateInfo& info)
{
	if(m_iterator >= m_defaultFrameTimesSize)
	{
		m_iterator = 0;
	}

	auto timeElapsed = GetElapsedTime(info.Clock);
	m_timeElapsedText.Text = L"Time elapsed: " + std::to_wstring(timeElapsed) + L" s";

	auto frameTime = GetDeltaTime(info.Clock);
	m_frameTimeText.Text = L"Frame time: " + std::to_wstring(frameTime) + L" s";

	
	m_frameTimes[m_iterator++] = frameTime;
	int averageFps = m_frameTimes.size() / std::accumulate(m_frameTimes.cbegin(), m_frameTimes.cend(), 0.0f);
	m_fpsText.Text = L"Fps: " + std::to_wstring(averageFps);
}

void Demo::TimeInfoDisplayer::OnDraw(const DE::DrawInfo& info)
{
	info.SpriteTextDrawer.EnqueueDrawCall(DE::SpriteFontName::SegoeUI_14, m_timeElapsedText);
	info.SpriteTextDrawer.EnqueueDrawCall(DE::SpriteFontName::SegoeUI_14, m_frameTimeText);
	info.SpriteTextDrawer.EnqueueDrawCall(DE::SpriteFontName::SegoeUI_14, m_fpsText);
}
