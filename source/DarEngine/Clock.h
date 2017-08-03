#pragma once

namespace DarEngine
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

		LONGLONG				GetStartTime() const;
		LONGLONG				GetActualTime() const;
		LONGLONG				GetLastTime() const;
		LONGLONG				GetFrequency() const;
		void					Update();
		void					Reset();
		
	private:
		LONGLONG		m_startTime{};
		LONGLONG		m_actualTime{};
		LONGLONG		m_lastTime{};
		LONGLONG		m_frequency{};
	};

	inline LONGLONG ComputeDeltaTime(const Clock& clock)
	{
		return (clock.GetActualTime() - clock.GetLastTime()) / clock.GetFrequency();
	}

	inline LONGLONG ComputeTotalTime(const Clock& clock)
	{
		return (clock.GetActualTime() - clock.GetStartTime()) / clock.GetFrequency();
	}
}
