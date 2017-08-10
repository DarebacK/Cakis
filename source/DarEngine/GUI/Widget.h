#pragma once
#include "SpriteBatch.h"

namespace DE
{
	namespace GUI
	{
		class Widget
		{
		public:
			explicit	Widget(DirectX::SpriteBatch* spriteBatch);
			~Widget();

		protected:
			DirectX::XMFLOAT2		m_position{};

		private:
			DirectX::SpriteBatch*	m_spriteBatch{ nullptr };
		};

		inline Widget::~Widget() {}
	}
}
