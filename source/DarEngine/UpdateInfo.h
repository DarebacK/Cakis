#pragma once

namespace DE
{
namespace Timing
{
	template<typename>
	class Clock;
}
}

namespace DE
{
	class UpdateInfo
	{
	public:
					UpdateInfo() = delete;
					UpdateInfo(const UpdateInfo& other) = delete;
		UpdateInfo&	operator=(const UpdateInfo& other) = delete;
					UpdateInfo(UpdateInfo&& other) = delete;
		UpdateInfo&	operator=(UpdateInfo&& other) = delete;
					~UpdateInfo() = default;

		const Timing::Clock<std::chrono::high_resolution_clock>&	HighResolutionClock;
		const Timing::Clock<std::chrono::system_clock>&				SystemClock;
		const DirectX::Keyboard::KeyboardStateTracker&				KeyboardStateTracker;
		DirectX::Keyboard::State									KeyboardState;
		
	};
}