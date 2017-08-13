#pragma once

namespace DE
{
	class Game;
}

namespace DE
{
	class SpriteText
	{
	public:
		std::wstring		Text{};
		DirectX::XMFLOAT2		Position{ 0.0f, 0.0f };
		DirectX::XMVECTORF32	Color{ DirectX::Colors::Black };
		float					Rotation{ 0.0f };
		DirectX::XMFLOAT2		Origin{ 0.0f, 0.0f };
		float					Scale{ 1.0f };
		DirectX::SpriteEffects	Effects{ DirectX::SpriteEffects_None };
		float					LayerDepth{ 0.0f };
	};

	enum class SpriteFontName
	{
		SegoeUI_14
	};

	class SpriteTextDrawer
	{
		friend Game;

	public:
		explicit			SpriteTextDrawer(ID3D11Device& device, ID3D11DeviceContext& deviceContext);
							SpriteTextDrawer(const SpriteTextDrawer& other) = delete;
		SpriteTextDrawer&	operator=(const SpriteTextDrawer& rhs) = delete;
							SpriteTextDrawer(SpriteTextDrawer&& other) = default;
		SpriteTextDrawer&	operator=(SpriteTextDrawer&& rhs) = default;
							~SpriteTextDrawer() = default;
		void				EnqueueDrawCall(const std::wstring& fontPath, const SpriteText& text);
		void				EnqueueDrawCall(SpriteFontName fontName, const SpriteText& text);
		
	private:
		static const std::wstring										m_fontDirectory;
		// contains default fonts contained in DarEngine, used in EnqueueDrawCall with SpriteFontName parameter
		static const std::unordered_map<SpriteFontName, std::wstring>	m_namesToPaths;
		// contains loaded spriteFonts which can be used acrossed multiple instances since
		// postprocessing matters only on the SpriteBatch
		static std::unordered_map<std::wstring, DirectX::SpriteFont>	m_pathsToFonts;

		ID3D11Device*			m_device;
		DirectX::SpriteBatch	m_spriteBatch;
		
		void					OnPreDraw();
		void					OnPostDraw();
	};
}