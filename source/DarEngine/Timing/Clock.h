#pragma once

#include "Diagnostics/Exception.h"

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
			auto			GetStartTimePoint() const noexcept { return m_startTimePoint; }
			auto			GetCurrentTimePoint() const noexcept { return m_actualTimePoint; }
			auto			GetPreviousTimePoint() const noexcept { return m_currentTimePoint; }

		private:
			typename ClockType::time_point				m_startTimePoint{};
			typename ClockType::time_point				m_currentTimePoint{};
			typename ClockType::time_point				m_actualTimePoint{};
		};

		template <typename clock_type>
		Clock<clock_type>::Clock()
		{
			Reset();
		}

		template <typename clock_type>
		void Clock<clock_type>::Update()
		{
			m_currentTimePoint = m_actualTimePoint;
			m_actualTimePoint = ClockType::now();
		}

		template <typename clock_type>
		void Clock<clock_type>::Reset()
		{
			m_startTimePoint = ClockType::now();
			m_currentTimePoint = m_startTimePoint;
			m_actualTimePoint = m_startTimePoint;
		}

		//Helper function which returns elapsed time since the clock's start (reset).
		template<typename ratio_type,
			typename value_type,
			typename clock_type>
			value_type GetElapsedTime(const clock_type& clock)
		{
			return std::chrono::duration_cast<std::chrono::duration<value_type, ratio_type>>(
				clock.GetCurrentTimePoint() - clock.GetStartTimePoint()
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
				clock.GetCurrentTimePoint() - clock.GetPreviousTimePoint()
				).count();
		}

		inline std::tm TimePointToLocalTime(const std::chrono::system_clock::time_point& timePoint)
		{
			std::time_t tempTime = std::chrono::system_clock::to_time_t(timePoint);
			struct tm returnValue;

			// returns zero if successful, which contradicts the cppreference page http://en.cppreference.com/w/c/chrono/localtime
			if(localtime_s(&returnValue, &tempTime))
			{
				throw DE::Diagnostics::Exception{ L"DE::Timing::TimePointToLocalTime() failed." };
			}

			return returnValue;
		}

		inline std::wstring TimeToWString(const std::tm& time,const std::wstring& format = L"%c")
		{
			std::wstringstream tempWStringStream;
			tempWStringStream << std::put_time(&time, format.c_str());

			return tempWStringStream.str();
		}
	}
}