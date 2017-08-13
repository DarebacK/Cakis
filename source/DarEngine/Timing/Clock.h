#pragma once

namespace DE
{
namespace Timing
{
	//TODO: description
	template<typename clock_type>
	class Clock
	{
	public:
		using			ClockType = clock_type;

						Clock();
						Clock(const Clock& other) = default;
						Clock(Clock&& other) noexcept = default;
						Clock& operator=(const Clock& other) = delete;
						Clock& operator=(Clock&& other) noexcept = default;
						~Clock() = default;

		void			Update();
		void			Reset();
		auto			GetStartTimePoint() const noexcept { return m_startTime; }
		auto			GetActualTimePoint() const noexcept { return m_actualTime; }
		auto			GetDeltaTimeDuration() const noexcept { return m_deltaTime; };
		
	private:
		typename ClockType::time_point	m_startTime;
		typename ClockType::time_point	m_actualTime;
		std::chrono::duration<float>	m_deltaTime	{ m_deltaTime.zero() };
	};

	template <typename clock_type>
	Clock<clock_type>::Clock()
	{
		Reset();
	}

	template <typename clock_type>
	void Clock<clock_type>::Update()
	{
		auto timeTemp = ClockType::now();
		m_deltaTime = timeTemp - m_actualTime;
		m_actualTime = timeTemp;
	}

	template <typename clock_type>
	void Clock<clock_type>::Reset()
	{
		m_startTime = ClockType::now();
		m_actualTime = m_startTime;
		m_deltaTime = m_deltaTime.zero();
	}

	template<typename clock_type>
	float GetDeltaTime(const Clock<clock_type>& clock)
	{
		return clock.GetDeltaTimeDuration().count();
	}

	template<typename clock_type>
	auto GetElapsedTime(const Clock<clock_type>& clock)
	{
		return (clock.GetActualTimePoint() - clock.GetStartTimePoint()).count();
	}
}
}
