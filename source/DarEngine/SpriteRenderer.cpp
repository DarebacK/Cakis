#include "stdafx.h"
#include "SpriteRenderer.h"
#include "Sprite.h"

DE::SpriteRenderer::SpriteRenderer(ID3D11DeviceContext& deviceContext)
	:m_spriteBatch( std::make_unique<DirectX::SpriteBatch>(&deviceContext))
{
}

void DE::SpriteRenderer::Draw(const Sprite& sprite) const
{
	m_spriteBatch->Draw(sprite.Texture.Get(), sprite.Position, sprite.SourceRectangle.get(), sprite.Color,
		sprite.Rotation, sprite.Origin, sprite.Scale, sprite.Effects, sprite.LayerDepth);
}

void DE::SpriteRenderer::OnPreDraw() const
{
	m_spriteBatch->Begin();
}

void DE::SpriteRenderer::OnPostDraw() const
{
	m_spriteBatch->End();
}
