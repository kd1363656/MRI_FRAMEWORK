#include "Fence.h"

FWK::Graphics::Fence::Fence() : 
	m_fenceEvent(nullptr),

	m_fenceValue(Constant::k_unusedFenceValue),

	m_fence(nullptr)
{}
FWK::Graphics::Fence::~Fence() 
{
	// イベントハンドルが作成されていなければreturn
	if (!m_fenceEvent) { return; }

	// イベントハンドルを開放
	CloseHandle(m_fenceEvent);

	m_fenceEvent = nullptr;
}

bool FWK::Graphics::Fence::Create(const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	// デバイスが存在しなければフェンスを作成することはできないのでreturn
	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、フェンスの作成に失敗しました。");
		return false;
	}

	// フェンスを作成する関数
	// CreateFence(フェンスの初期値、
	//			   フェンス作成時の追加設定、
	//			   受け取りたいCOMインターフェース型のID、
	//			   作成結果のポインタを書き込むアドレス);

	auto l_hr = l_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	// CPU側が「GPUの処理完了を待つ」ときに使うイベントオブジェクトを作成する
	// CreateEvent(セキュリティ属性、
	//			   manualResetにするかどうか(FALSEなので自動リセットイベント、一度シグナル状態になって待機が解除されたら、自動で非シグナル状態に戻る、
	//			   作成直後にシグナル状態にするかどうか、
	//			   イベント名);

	m_fenceEvent = CreateEvent(nullptr,
							   FALSE,
							   FALSE,
							   nullptr);

	if (!m_fenceEvent)
	{
		assert(false && "イベントの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::Fence::WaitForFenceValueIfNeeded(const UINT64& a_fenceValue)
{
	// フェンスが存在しなければGPU完了確認はできない
	if (!m_fence)
	{
		assert(false && "フェンスの作成に失敗しておりフェンス値まで待つかどうかの判定に失敗しました。");
		return;
	}

	// GPUの完了値が、引数で渡された目標フェンス値以上なら、
	// その地点までの処理は完了済みと見なせるのでreturn
	if (const auto& l_completedValue = m_fence->GetCompletedValue();
		l_completedValue >= a_fenceValue) 
	{
		return; 
	}

	// GPUがa_fenceValueに到達したらm_fenceEventを通知状態にする関数
	// SetEventOnCompletion(CPUが待ちたい目標のフェンス値、
	//						GPUが完了通知を受け取るイベント);

	const auto l_hr = m_fence->SetEventOnCompletion(a_fenceValue, m_fenceEvent);

	if (FAILED(l_hr))
	{
		assert(false && "フェンス完了イベントの設定に失敗しました。");
		return;
	}

	// m_fenceEventが通知されるまでCPUを待機させる
	// INFINITEは時間制限なしで待つ指定
	// WaitForSingleObject(待機対象のイベント、
	//					   待機時間);

	const auto l_waitResult = WaitForSingleObject(m_fenceEvent, INFINITE);

	if (l_waitResult != WAIT_OBJECT_0)
	{
		assert(false && "フェンス待機に失敗しました。");
		return;
	}
}