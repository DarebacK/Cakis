#include "stdafx.h"
#include "Sprite.h"

DE::SpriteDrawer::SpriteDrawer(ID3D11DeviceContext& deviceContext)
	:m_spriteBatch{ &deviceContext }
{
}

void DE::SpriteDrawer::QueueDrawCall(const Sprite& sprite)
{
	m_spriteBatch.Draw(sprite.Texture.Get(), sprite.Position, sprite.SourceRectangle.get(), sprite.Color,
		sprite.Rotation, sprite.Origin, sprite.Scale, sprite.Effects, sprite.LayerDepth);
}

void DE::SpriteDrawer::OnPreDraw()
{
	m_spriteBatch.Begin();
}

void DE::SpriteDrawer::OnPostDraw()
{
	m_spriteBatch.End();
}
