#pragma once

namespace DE
{
namespace Timing
{
	//TODO: description
	class Clock
	{
	public:
		using			ClockType = std::chrono::high_resolution_clock;

						Clock();
						Clock(const Clock& other) = default;
						Clock(Clock&& other) noexcept = default;
						Clock& operator=(const Clock& other) = delete;
						Clock& operator=(Clock&& other) noexcept = default;
						~Clock() = default;

		void			Update();
		void			Reset();
		auto			GetStartTime() const noexcept { return m_startTime; }
		auto			GetActualTime() const noexcept { return m_actualTime; }
		auto			GetDeltaTime() const noexcept { return m_deltaTime; }
		
	private:
		ClockType::time_point			m_startTime;
		ClockType::time_point			m_actualTime;
		std::chrono::duration<float>	m_deltaTime	{ m_deltaTime.zero() };
	};
}
}
