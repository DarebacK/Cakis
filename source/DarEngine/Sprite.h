#pragma once

namespace DE
{
	struct Sprite
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	Texture{ nullptr };
		DirectX::XMFLOAT2									Position{0.0f, 0.0f};
		std::unique_ptr<RECT>								SourceRectangle{ nullptr };
		DirectX::XMVECTORF32								Color{ 0.0f, 0.0f, 0.0f, 0.0f};
		float												Rotation{ 0.0f };
		DirectX::XMFLOAT2									Origin{ 0.0f, 0.0f };
		float												Scale{ 1.0f };
		DirectX::SpriteEffects								Effects { DirectX::SpriteEffects_None};
		float												LayerDepth{ 0.0f };
	};
}
