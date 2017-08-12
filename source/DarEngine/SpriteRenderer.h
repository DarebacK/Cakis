#pragma once

namespace DE
{
	class Sprite;
}

namespace DirectX
{
	class SpriteBatch;
}

namespace DE
{
	class SpriteRenderer
	{
	public:
		explicit	SpriteRenderer(ID3D11DeviceContext& deviceContext);
		void		OnPreDraw() const;
		void		Draw(const Sprite& sprite) const;
		void		OnPostDraw() const;
		
	private:
		std::unique_ptr<DirectX::SpriteBatch>	m_spriteBatch{ nullptr };
	};
}