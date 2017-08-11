#pragma once

namespace DE
{
namespace Timing
{
	class Clock;
}
}

namespace DE
{
	struct UpdateInfo
	{
		explicit UpdateInfo(const Timing::Clock& clock)
			:Clock{clock}
		{}

		const Timing::Clock&	Clock;
	};
}