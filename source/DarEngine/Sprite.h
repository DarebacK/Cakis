#pragma once

namespace DE
{
	class Game;
}

namespace DE
{
	class Sprite
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	Texture{ nullptr };
		DirectX::XMFLOAT2									Position{ 0.0f, 0.0f };
		std::unique_ptr<RECT>								SourceRectangle{ nullptr };
		DirectX::XMVECTORF32								Color{ 0.0f, 0.0f, 0.0f, 1.0f };
		float												Rotation{ 0.0f };
		DirectX::XMFLOAT2									Origin{ 0.0f, 0.0f };
		float												Scale{ 1.0f };
		DirectX::SpriteEffects								Effects{ DirectX::SpriteEffects_None };
		float												LayerDepth{ 0.0f };
	};

	class SpriteDrawer
	{
		friend Game;

	public:
		explicit	SpriteDrawer(ID3D11DeviceContext& deviceContext);
		void		QueueDrawCall(const Sprite& sprite);
	
	private:
		DirectX::SpriteBatch	m_spriteBatch;

		void					OnPreDraw();
		void					OnPostDraw();
	};
}