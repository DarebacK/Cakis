#pragma once

namespace DE
{
namespace Input
{
	// wraps Windows Virtual-Key Codes, defines supported keys
	// keys are directly mapped to Windows Virtual-Key Codes (https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx)
	enum class KeyCode : WPARAM
	{
		// 0x00 reserved

		MouseLeft		= 0x01, Mouse1	= MouseLeft,
		MouseMiddle		= 0x02, Mouse2	= MouseMiddle,
		MouseRight		= 0x03, Mouse3	= MouseRight,
		Mouse4			= 0x04,
		Mouse5			= 0x05,
		ArrowLeft		= 0x25,
		ArrowUp			= 0x26,
		ArrowRight		= 0x27,
		ArrowDown		= 0x28
	};
}
}