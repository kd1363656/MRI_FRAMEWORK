#include "Fence.h"

FWK::Render::Fence::Fence(const Hardware& a_hardware, const GraphicsCommandQueue& a_graphicsCommandQueue) : 
	k_hardware            (a_hardware),
	k_graphicsCommandQueue(a_graphicsCommandQueue),
	m_fenceEvent          (nullptr),
	m_fenceValue		  (0ULL)
{}
FWK::Render::Fence::~Fence()
{
	// フェンスクラスがなくなるということはアプリケーションが終了する
	// ということなのでGPUと完全同期をとってDirectX12のDeviceが
	// Releaseされてもいいようにする
	WaitForGPUIdle();
}

void FWK::Render::Fence::Init()
{
	m_fence = nullptr;

	m_fenceEvent = nullptr;

	m_fenceValue = CommonConstant::k_initialFenceValue;
}
bool FWK::Render::Fence::Create()
{
	const auto& l_device = k_hardware.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、フェンスの作成に失敗しました。");
		return false;
	}

	auto l_hr = l_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_fenceEvent)
	{
		assert(false && "イベントの作成に失敗しました。");
		return false;
	}
	
	return true;
}

void FWK::Render::Fence::BeginFrame(std::vector<FrameResource>& a_frameResourceList, const SwapChain& a_swapChain)
{
	const UINT  l_currentBackBufferIndex = a_swapChain.GetCurrentBackBufferIndex();

	const auto& l_frameResource = a_frameResourceList[l_currentBackBufferIndex];

	// ※重要
	// フレームの最初で現在使用しようとしているコマンドアロケータの再利用ができるかどうかを確認
	// もしフェンス値が"GetCompletedValue"を超えていたら"GPU"と同期をとる
	// "GetCompletedValue"以下なら同期を取らない(同じコマンドアロケータを使わない限り"GPU"との同期が発生しないため)
	if (m_fence->GetCompletedValue() >= l_frameResource.GetFenceValue()) { return; }

	// "GPU"のフェンス値が"GetCompletedValue"以上になったらイベントを発火
	m_fence->SetEventOnCompletion(l_frameResource.GetFenceValue(), m_fenceEvent);

	// 指定したフェンスがシグナル状態になるまで待つ
	WaitForSingleObject(m_fenceEvent, INFINITE);
}
void FWK::Render::Fence::EndFrame(std::vector<FrameResource>& a_frameResourceList, const SwapChain& a_swapChain, const GraphicsCommandQueue& a_graphicsCommandQueue)
{
	const UINT  l_currentBackBufferIndex = a_swapChain.GetCurrentBackBufferIndex ();
	const auto& l_graphicsCommandQueue   = a_graphicsCommandQueue.GetCommandQueue();

	if (!m_fence)
	{
		assert(false && "フェンスが作成されておらずGPUとの同期処理が行えませんでした。");
		return;
	}

	if (!l_graphicsCommandQueue)
	{
		assert(false && "コマンドキューが作成されておらずGPUとの同期処理が行えませんでした。");
		return;
	}

	// コマンドアロケータリストよりも現在取得したバックバッファの数が多ければ"assert"
	if (a_frameResourceList.size() <= l_currentBackBufferIndex)
	{
		assert(false && "フレームリソースの数よりも取得したバックバッファのインデックスが容量を超えています。");
		return;
	}

	// 現在使用しているフレームリソースを取得
	auto& l_frameResource = a_frameResourceList[l_currentBackBufferIndex];

	// "FenceValue"を進めて、子のフレームの完了目標として保存
	++m_fenceValue;

	// ※重要
	// 更新したフェンス値を持たせて置く、こうすることで現在使用されたコマンドアロケータが"l_commandAllocator[0]"のフェンス値が"1"で
	// "l_commandAllocator[1]"のフェンス値が"10"の場合"l_commandAllocator[1]"から"l_commandAllocator[0]"を使用する流れでも
	// フェンス値が"m_fence->GetCompletedValue"を超えない限り"GPU"との処理並列性を壊さずに処理が可能になる
	l_frameResource.SetFenceValue(m_fenceValue);

	// "GPU"が到達すべき目標値を更新
	l_graphicsCommandQueue->Signal(m_fence.Get(), m_fenceValue);
}

void FWK::Render::Fence::WaitForGPUIdle()
{
	if (!m_fence) { return; }

	const auto& l_graphicsCommandQueue = k_graphicsCommandQueue.GetCommandQueue();

	if (!l_graphicsCommandQueue)
	{
		assert(false && "コマンドキューが作製されておらず、\"GPU\"との同期が取れません。");
		return;
	}

	// フェンス値を更新し更新結果を目標フェンス値として持つ
	++m_fenceValue;

	const UINT64 l_targetFenceValue = m_fenceValue;

	// キューの処理がここまで進んだらフェンス値を更新せよという指示
	l_graphicsCommandQueue->Signal(m_fence.Get(), l_targetFenceValue);

	// GPUがまだ到達してなければ待機
	if (m_fence->GetCompletedValue() < l_targetFenceValue)
	{
		// GPUのフェンス値がGetCompletedValue以上になったらイベントを発火
		m_fence->SetEventOnCompletion(l_targetFenceValue, m_fenceEvent);

		// CPUスレッドをスリープ(完全同期)
		WaitForSingleObject(m_fenceEvent, INFINITE);	 
	}
}