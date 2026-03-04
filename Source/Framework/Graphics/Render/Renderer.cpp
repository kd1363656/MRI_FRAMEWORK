#include "Renderer.h"

FWK::Graphics::Renderer::Renderer(const Hardware& a_hardware, const ShaderCompiler& a_shaderCompiler) :
	k_hardware          (a_hardware),
	m_directCommandQueue(a_hardware.GetDevice()),
	m_directCommandList (a_hardware.GetDevice()),
	m_renderArea        (),
	m_rootSignature     (a_hardware.GetDevice()),
	m_pipelineState     (a_hardware.GetDevice(), a_shaderCompiler),
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

	m_renderArea.Init   ();
	m_rootSignature.Init();
	m_pipelineState.Init();

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

	if (!m_rootSignature.Create())
	{
		assert(false && "ルートシグネチャの作成に失敗しました。");
		return false;
	}

	if (!m_pipelineState.Create(m_rootSignature))
	{
		assert(false && "パイプラインステートの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::Renderer::PostCreateSetup(const SwapChain& a_swapChain)
{
	// 次使用するコマンドリスト、アロケータをリセット
	ResetCommandObjects();

	if (!m_renderArea.SetupRenderArea(a_swapChain))
	{
		assert(false && "描画エリアの作成に失敗。");
		return;
	}
}

void FWK::Graphics::Renderer::BeginFrame(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap)
{
	if (m_frameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためBeginFrame処理が行えませんでした。。");
		return;
	}

	const auto& l_commandAllocator = m_frameResourceList[m_frameIndex].GetDirectCommandAllocator();

	// コマンドアロケータがGPU処理が終わっているかどうかを確かめGPUの処理が終わっていなければWait
	m_directCommandQueue.EnsureAllocatorAvailable(l_commandAllocator);

	// バックバッファの状態遷移(Present -> Resource)
	m_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 描画するためのレンダーターゲットをセットしクリア
	m_directCommandList.SetupRenderTarget(a_swapChain, a_rtvDescriptorHeap);

	// ビューポートとシザー矩形をセット
	m_directCommandList.SetupRenderArea(m_renderArea);

	// ルートシグネチャをセット
	m_directCommandList.SetupRootSignature(m_rootSignature);

	// パイプラインステートをセット
	m_directCommandList.SetupPipelineState(m_pipelineState);
}
void FWK::Graphics::Renderer::Draw()
{
	// メッシュシェーダーに転送
	m_directCommandList.DispatchMesh(1U, 1U, 1U);
}
void FWK::Graphics::Renderer::EndFrame(const SwapChain& a_swapChain)
{
	if (m_frameIndex >= m_frameResourceList.size())
	{
		assert(false && "フレームリソースの容量を超えたインデックスのためBeginFrame処理が行えませんでした。");
		return;
	}

	auto& l_commandAllocator = m_frameResourceList[m_frameIndex].GetWorkDirectCommandAllocator();

	// バックバッファの状態遷移(Present -> Resource)
	m_directCommandList.TransitionRenderTargetResource(a_swapChain, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// GPUに描画命令を出す
	m_directCommandQueue.ExecuteCommandLists(m_directCommandList);

	// フェンス値を更新
	m_directCommandQueue.SignalAndTracAllocator(l_commandAllocator);

	ResetCommandObjects();

	a_swapChain.Present();

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