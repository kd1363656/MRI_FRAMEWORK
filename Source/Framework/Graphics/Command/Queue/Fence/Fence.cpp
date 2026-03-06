#include "Fence.h"

FWK::Graphics::Fence::Fence(const Device& a_device) :
	k_device		  (a_device),
	m_fenceEvent      (nullptr),
	m_fenceValue      (0ULL)
{}
FWK::Graphics::Fence::~Fence()
{
	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;
	}
}

void FWK::Graphics::Fence::Init()
{
	m_fence = nullptr;

	m_fenceEvent = nullptr;

	m_fenceValue = CommonConstant::k_initialFenceValue;
}
bool FWK::Graphics::Fence::Create()
{
	const auto& l_device = k_device.GetDevice();

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

void FWK::Graphics::Fence::WaitForFenceValue(const UINT64& a_fenceValue)
{
	if (!m_fence)
	{
		assert(false && "フェンスの作成に失敗しておりコマンドアロケータの使用可能かどうかの選定に失敗しました。");
		return;
	}

	const UINT64& l_completedValue = m_fence->GetCompletedValue();

	// フェンス値が0ならフェンス値が更新されていないのでreturn
	if (l_completedValue == CommonConstant::k_unusedFenceValue) { return; }

	// フレームの最初で現在使用しようとしているコマンドアロケータの再利用ができるかどうかを確認
	// GPUの完了値がまだ足りない場合にのみ同期をとることでCPUとGPUの並列処理性を発揮することができる
	if (l_completedValue >= a_fenceValue) { return; }

	// イベントを発火することを予約
	m_fence->SetEventOnCompletion(a_fenceValue , m_fenceEvent);

	WaitForSingleObject(m_fenceEvent, INFINITE);
}
void FWK::Graphics::Fence::WaitForGPUIdle(const ComPtr<ID3D12CommandQueue> a_commandQueue)
{
	if (!m_fence) { return; }

	if (!a_commandQueue)
	{
		assert(false && "ダイレクトコマンドキューが作製されておらず、GPUとの同期が取れません。");
		return;
	}

	// フェンス値を更新し更新結果を目標フェンス値として持つ
	++m_fenceValue;

	const UINT64 l_targetFenceValue = m_fenceValue;

	// コマンドキュー内でこの位置までの命令が実行完了したら
	// フェンス値をm_fenceValueに更新する命令をGPUに追加
	a_commandQueue->Signal(m_fence.Get(), l_targetFenceValue);

	// GPUがまだ到達してなければ待機
	if (m_fence->GetCompletedValue() < l_targetFenceValue)
	{
		// GPUのフェンス値がGetCompletedValue以上になったらイベントを発火
		m_fence->SetEventOnCompletion(l_targetFenceValue, m_fenceEvent);

		// CPUスレッドをスリープ(完全同期)
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

UINT64 FWK::Graphics::Fence::GetCompletedFenceValue() const
{
	if (!m_fence) 
	{
		assert(false && "フェンスが作成されておらずCompletedFanceValueの取得に失敗しました。");
		return 0ULL; 
	}

	return m_fence->GetCompletedValue();
}