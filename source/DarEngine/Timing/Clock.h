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
		auto			GetPreviousTimePoint() const noexcept { return m_previousTime; }
		
	private:
		typename ClockType::time_point				m_startTime{};
		typename ClockType::time_point				m_previousTime{};
		typename ClockType::time_point				m_actualTime{};
	};

	template <typename clock_type>
	Clock<clock_type>::Clock()
	{
		Reset();
	}

	template <typename clock_type>
	void Clock<clock_type>::Update()
	{
		m_previousTime = m_actualTime;
		m_actualTime = ClockType::now();
	}

	template <typename clock_type>
	void Clock<clock_type>::Reset()
	{
		m_startTime = ClockType::now();
		m_previousTime = m_startTime;
		m_actualTime = m_startTime;
	}

	//Helper function which returns elapsed time since the clock's start (reset).
	template<typename ratio_type,
		typename value_type,
		typename clock_type>
		value_type GetElapsedTime(const clock_type& clock)
	{
		return std::chrono::duration_cast<std::chrono::duration<value_type, ratio_type>>(
			clock.GetActualTimePoint() - clock.GetStartTimePoint()
			).count();
	}

	template<typename clock_type>
	auto GetElapsedTime(const Clock<clock_type>& clock)
	{
		return GetElapsedTime<
			typename clock_type::time_point::duration::period, 
			typename clock_type::time_point::duration::rep>
		(clock);
	}

	template<typename ratio_type = std::ratio<1>,
			typename value_type = float,
			typename clock_type>
	auto GetDeltaTime(const Clock<clock_type>& clock) 
	{
		return std::chrono::duration_cast<std::chrono::duration<value_type, ratio_type>>(
			clock.GetActualTimePoint() - clock.GetPreviousTimePoint()
			).count();
	}
}
}
