#pragma once

namespace DE
{
	class SpriteDrawer;
	class SpriteTextDrawer;
}

namespace DE
{
	class DrawInfo
	{
	public:
					DrawInfo() = delete;
					DrawInfo(const DrawInfo& other) = delete;
		DrawInfo&	operator=(const DrawInfo& rhs) = delete;
					DrawInfo(DrawInfo&& other) = delete;
		DrawInfo&	operator=(DrawInfo&& rhs) = delete;
					~DrawInfo() = default;

		SpriteDrawer&		SpriteDrawer;
		SpriteTextDrawer&	SpriteTextDrawer;
	};
}