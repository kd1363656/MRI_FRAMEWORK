#include "SpriteStandardPerObjectDrawRequest.h"

void FWK::Graphics::SpriteStandardPerObjectDrawRequest::BeginFrame()
{
	// 前フレームのSprite描画申請を消す
	m_drawRequestPerObjectList.BeginFrame();
}

void FWK::Graphics::SpriteStandardPerObjectDrawRequest::AddDrawRequestPerObject(const std::shared_ptr<Struct::SpriteStandardPerObjectDrawRequestData>& a_drawRequestData)
{
	m_drawRequestPerObjectList.AddDrawRequestPerObject(a_drawRequestData);
}