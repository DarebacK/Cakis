#pragma once

#include "Component.h"
#include "SpriteText.h"

namespace DirectX {
	struct XMFLOAT2;
}

namespace Demo
{
	class TimeInfoDisplayer : public DE::Component
	{
	public:
		TimeInfoDisplayer();

	private:
		static const size_t m_defaultFrameTimesSize;

		std::vector<float>	m_frameTimes = std::vector<float>(m_defaultFrameTimesSize);
		DE::SpriteText		m_fpsText{};
		DE::SpriteText		m_frameTimeText{};
		DE::SpriteText		m_timeElapsedText{};
		int					m_iterator{ 0 };


		void OnUpdate(const DE::UpdateInfo& info) override;
		void OnDraw(const DE::DrawInfo& info) override;
	};
}

