#include "DrawSpriteStandardCommand.h"

void FWK::Graphics::DrawSpriteStandardCommand::BeginFrame()
{
	m_spriteStandardCommandList.clear();
}

void FWK::Graphics::DrawSpriteStandardCommand::Draw(const Renderer& a_renderer, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, TextureSystem& a_textureSystem)
{

}

void FWK::Graphics::DrawSpriteStandardCommand::RequestSpriteStandardDraw(const Struct::SpriteDrawCommand& a_spriteStandardDraw)
{
	m_spriteStandardCommandList.emplace_back(a_spriteStandardDraw);
}