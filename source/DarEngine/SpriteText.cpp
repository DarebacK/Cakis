#include "stdafx.h"
#include "SpriteText.h"

const std::wstring DE::SpriteTextDrawer::m_fontDirectory{ L"content\\DarEngine\\fonts\\" };

const std::unordered_map<DE::SpriteFontName, std::wstring> DE::SpriteTextDrawer::m_namesToPaths{
	{SpriteFontName::SegoeUI_14, m_fontDirectory + L"SegoeUI_14.spritefont"}
};

std::unordered_map<std::wstring, DirectX::SpriteFont> DE::SpriteTextDrawer::m_pathsToFonts{};

DE::SpriteTextDrawer::SpriteTextDrawer(ID3D11Device& device, ID3D11DeviceContext& deviceContext)
	: m_device{&device}, m_spriteBatch{ &deviceContext }
{
}

void DE::SpriteTextDrawer::OnPreDraw()
{
	m_spriteBatch.Begin();
}

void DE::SpriteTextDrawer::OnPostDraw()
{
	m_spriteBatch.End();
}

void DE::SpriteTextDrawer::EnqueueDrawCall(const std::wstring& fontPath, const SpriteText& text)
{
	auto iterator = m_pathsToFonts.try_emplace(fontPath, m_device, fontPath.c_str());
	iterator.first->second.DrawString(&m_spriteBatch, text.Text.c_str(), text.Position, text.Color, text.Rotation,
		text.Origin, text.Scale, text.Effects, text.LayerDepth);
}

void DE::SpriteTextDrawer::EnqueueDrawCall(SpriteFontName name, const SpriteText& text)
{
	auto path = m_namesToPaths.at(name);
	EnqueueDrawCall(path, text);
}
