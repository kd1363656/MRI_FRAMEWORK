#include "Renderer.h"

FWK::Graphics::Renderer::Renderer(const Hardware& a_hardware) :
	k_hardware          (a_hardware),
	m_directCommandQueue(a_hardware.GetDevice()),
	m_directCommandList (a_hardware.GetDevice()),
	m_frameIndex        (0ULL)
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

	m_frameIndex = 0ULL;
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

void FWK::Graphics::Renderer::PostCreateSetup()
{
	// 全てのコマンドアロケータをリセット
	if (m_frameResourceList.empty())
	{
		assert(false && "フレームリソースの要素が存在せずリセット処理を行えませんでした。");
		return;
	}

	const auto& l_commandAllocator = m_frameResourceList[m_frameIndex].GetDirectCommandAllocator();

	l_commandAllocator.Reset ();
	m_directCommandList.Reset(l_commandAllocator);
}

void FWK::Graphics::Renderer::BeginFrame(const SwapChain& a_swapChain)
{
	if (m_frameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためBeginFrame処理が行えませんでした。。");
		return;
	}

	const auto& l_commandAllocator = m_frameResourceList[m_frameIndex].GetDirectCommandAllocator();

	m_directCommandQueue.EnsureAllocatorAvailable(l_commandAllocator);
}
void FWK::Graphics::Renderer::EndFrame(const SwapChain& a_swapChain)
{
	if (m_frameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためBeginFrame処理が行えませんでした。");
		return;
	}

	auto& l_commandAllocator = m_frameResourceList[m_frameIndex].GetWorkDirectCommandAllocator();

	m_directCommandQueue.SignalAndTracAllocator(l_commandAllocator);

	// 容量を超えないように次のフレームで使用するインデックスを計算
	m_frameIndex = (m_frameIndex + 1U) % k_frameCount;
}

void FWK::Graphics::Renderer::ResetCommandObjects()
{
	if (m_frameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためリセット処理が行えませんでした。");
		return;
	}

	const auto& l_directCommandAllocator = m_frameResourceList[m_frameIndex].GetDirectCommandAllocator();

	l_directCommandAllocator.Reset();
	m_directCommandList.Reset     (l_directCommandAllocator);
}