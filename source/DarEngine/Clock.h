#pragma once

namespace DE
{
	class Clock
	{
	public:
						Clock();
						Clock(const Clock& other) = delete;
						Clock(Clock&& other) noexcept = default;
						Clock& operator=(const Clock& other) = delete;
						Clock& operator=(Clock&& other) noexcept = default;
						~Clock() = default;

		void			Update();
		void			Reset();
		
	private:
		LONGLONG		m_startTime{};
		LONGLONG		m_actualTime{};
		LONGLONG		m_lastTime{};
		LONGLONG		m_frequency{};

		friend LONGLONG ComputeDeltaTime(const Clock& clock)
		{
			return (clock.m_actualTime - clock.m_lastTime) / clock.m_frequency;
		}
		friend LONGLONG ComputeTotalTime(const Clock& clock)
		{
			return (clock.m_actualTime - clock.m_startTime) / clock.m_frequency;
		}
	};
}
