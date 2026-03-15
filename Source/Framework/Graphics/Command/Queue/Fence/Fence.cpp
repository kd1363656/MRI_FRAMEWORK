#include "Fence.h"

void FWK::Graphics::Fence::Init()
{
	m_fence.Reset();

	m_fenceEvent = nullptr;

	m_fenceValue = CommonConstant::k_initialFenceValue;
}
bool FWK::Graphics::Fence::Create(const Device& a_device)
{
	const auto& l_device = a_device.GetDevice();

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
	//			   manual restにするかどうか(FALSEなので自動リセットイベント、一度シグナル状態になって待機が解除されたら、自動で見シグナル状態に戻る、
	//			   作製直後に寝具なる状態にするかどうか、
	//			   イベント名);

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
	// フェンスが存在しなければGPU完了確認はできない
	if (!m_fence)
	{
		assert(false && "フェンスの作成に失敗しておりコマンドアロケータの使用可能かどうかの選定に失敗しました。");
		return;
	}

	// GPUがここまで完了しました、という現在のフェンス値を取得する
	// たとえばGPUがフェンス値5まで到達していれば、5以下の仕事は完了済みと判断できる
	const UINT64& l_completedValue = m_fence->GetCompletedValue();

	// フェンスが未使用値を返した場合は安全のためにぬける
	if (l_completedValue == CommonConstant::k_unusedFenceValue) { return; }

	// 既に目標値まで完了しているなら待機不要
	if (l_completedValue >= a_fenceValue) { return; }

	// GPUがa_fenceValueに到達したらm_fenceEventを通知状態にする関数
	// SetEventOnCompletion(CPUが待ちたい目標のフェンス値、
	//						GPUが完了通知を受け取るイベント);

	m_fence->SetEventOnCompletion(a_fenceValue, m_fenceEvent);

	// m_fenceEventが通知されるアでCPUを待機させる
	// INFINITEは時間制限なしで待つ指定
	// WaitForSingleObject(待機対象のイベント、
	//					   待機時間);

	WaitForSingleObject(m_fenceEvent, INFINITE);
}
void FWK::Graphics::Fence::WaitForGPUIdle(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& a_commandQueue)
{
	// フェンスが存在しなければGPU完了確認はできない
	if (!m_fence)
	{
		assert(false && "フェンスの作成に失敗しておりコマンドアロケータの使用可能かどうかの選定に失敗しました。");
		return;
	}

	if (!a_commandQueue)
	{
		assert(false && "コマンドキューが作製されておらず、GPUとの同期が取れません。");
		return;
	}

	// 今回の待機用に新しいフェンス値を発行する
	// 同じ値を使いまわすとどこまでの処理完了を待っているのか分からなくなるため
	++m_fenceValue;

	const UINT64& l_targetFenceValue = m_fenceValue;

	// コマンドキューに対して「命令したGPU処理が終わったら、m_fenceの値をl_targetFenceValueに更新してください」
	// と命令をする関数
	// Signal(更新対象のフェンスオブジェクト、
	//		  GPU完了時に設定するフェンス値);

	auto l_hr = a_commandQueue->Signal(m_fence.Get(), l_targetFenceValue);

	// Signal命令に失敗したらreturn
	if (FAILED(l_hr))
	{
		assert(false && "コマンドキューへのフェンスシグナルに失敗しました。");
		return;
	}

	WaitForFenceValue(l_targetFenceValue);
}