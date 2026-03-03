#include "Renderer.h"

FWK::Graphics::Renderer::Renderer(const Hardware& a_hardware) :
	k_hardware          (a_hardware),
	m_directCommandQueue(a_hardware.GetDevice()),
	m_directCommandList (a_hardware.GetDevice()),
	m_currentFrameIndex (0ULL)
{}
FWK::Graphics::Renderer::~Renderer() = default;

void FWK::Graphics::Renderer::Init()
{
	m_directCommandQueue.Init();
	m_directCommandList.Init ();

	// フレームリソースの要素数を確保
	m_frameResourceList.reserve(k_frameCount);

	for (std::size_t l_i = 0ULL; l_i < k_frameCount; ++l_i)
	{
		m_frameResourceList.emplace_back(k_hardware.GetDevice());

		// 確保が終わってから初期化
		m_frameResourceList[l_i].Init();
	}

	m_currentFrameIndex = 0ULL;
}
bool FWK::Graphics::Renderer::Create()
{
	if (!m_directCommandQueue.Create())
	{
		assert(false && "ダイレクトコマンドキューの作成に失敗。");
		return false;
	}

	if (!m_directCommandList.Create())
	{
		assert(false && "ダイレクトコマンドリストの作成に失敗。");
		return false;
	}

	for (auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource.Create())
		{
			assert(false && "フレームリソースの作成に失敗しました。");
			return false;
		}
	}

	return true;
}

void FWK::Graphics::Renderer::PostCreateSetup(const SwapChain& a_swapChain)
{
	// 全てのコマンドアロケータをリセット
	for (const auto& l_frameResource : m_frameResourceList)
	{
		l_frameResource.GetDirectCommandAllocator().Reset();
	}
}

void FWK::Graphics::Renderer::BeginFrame(const SwapChain& a_swapChain)
{
	// 容量を超えないように現在のフレームリソースで使用するインデックスを取得
	m_currentFrameIndex = (m_currentFrameIndex + 1U) % k_frameCount;

	if (m_currentFrameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためBeginFrame処理が行えませんでした。。");
		return;
	}


}
void FWK::Graphics::Renderer::EndFrame(const SwapChain& a_swapChain)
{

}

void FWK::Graphics::Renderer::ResetCommandObjects()
{
	if (m_currentFrameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためリセット処理が行えませんでした。。");
		return;
	}

	const auto& l_directCommandAllocator = m_frameResourceList[m_currentFrameIndex].GetDirectCommandAllocator();

	l_directCommandAllocator.Reset();
	m_directCommandList.Reset     (&l_directCommandAllocator);
}