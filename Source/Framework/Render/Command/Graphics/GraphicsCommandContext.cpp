#include "GraphicsCommandContext.h"

FWK::Render::GraphicsCommandContext::GraphicsCommandContext(const Hardware& a_hardware) :
	k_hardware            (a_hardware),
	m_graphicsCommandQueue(*this),
	m_graphicsCommandList (*this)
{}
FWK::Render::GraphicsCommandContext::~GraphicsCommandContext() = default;

void FWK::Render::GraphicsCommandContext::Init()
{
	m_graphicsCommandQueue.Init();
	m_graphicsCommandList.Init ();
}
bool FWK::Render::GraphicsCommandContext::Create()
{
	if (!m_graphicsCommandQueue.Create())
	{
		assert(false && "グラフィックスコマンドキューの作成に失敗しました。");
		return false;
	}

	if (!m_graphicsCommandList.Create())
	{
		assert(false && "グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}