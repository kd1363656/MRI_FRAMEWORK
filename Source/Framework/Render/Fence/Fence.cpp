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