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
		LONGLONG		GetStartTime() const noexcept { return m_startTime; }
		LONGLONG		GetActualTime() const noexcept { return m_actualTime; }
		LONGLONG		GetDeltaTime() const noexcept { return m_deltaTime; }
		LONGLONG		GetTotalTime() const noexcept { return m_totalTime; }
		
	private:
		LONGLONG		m_startTime{ 0 };
		LONGLONG		m_actualTime{ 0 };
		LONGLONG		m_frequency{ -1 };
		LONGLONG		m_deltaTime{ 0 };
		LONGLONG		m_totalTime{ 0 };
	};
}
