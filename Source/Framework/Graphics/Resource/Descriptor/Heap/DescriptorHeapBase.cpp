#include "DescriptorHeapBase.h"

FWK::Graphics::DescriptorHeapBase::DescriptorHeapBase(const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType, const bool a_isShaderVisibleSupported, const bool a_isShaderVisible) : 
	k_createDescriptorHeapType(a_createDescriptorHeapType),

	k_isShaderVisibleSupported(a_isShaderVisibleSupported),

	m_descriptorHeap(nullptr),

	m_cpuStart({}),
	m_gpuStart({}),

	m_descriptorCapacity(0U),
	m_descriptorSize    (0U),

	m_isShaderVisible(a_isShaderVisible)
{}
FWK::Graphics::DescriptorHeapBase::~DescriptorHeapBase() = default;

bool FWK::Graphics::DescriptorHeapBase::Create(const UINT a_descriptorCapacity, const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタ数0のヒープは意味がないので失敗扱い
	if (a_descriptorCapacity == 0U)
	{
		assert(false && "作成するディスクリプタ数が0です。");
		return false;
	}

	// 作成するディスクリプタヒープのタイプによってシェーダー可視性をサポートするかどうかを判別する
	// もしシェーダー可視性がサポートされていないならここで必ずfalseにする
	if (!k_isShaderVisibleSupported && m_isShaderVisible)
	{
		assert(false && "このディスクリプタヒープ種別はShaderVisibleをサポートしていません、派生クラスのコンストラクタを見直してください。");
		return false;
	}

	// ディスクリプタヒープ作成設定を入れる構造体
	auto l_desc = D3D12_DESCRIPTOR_HEAP_DESC();

	// このヒープの種類を設定する(RTV / DSV / CBV_SRV_UAV / SAMPLERの内どれか)
	l_desc.Type = k_createDescriptorHeapType;

	// このヒープに何個ディスクリプタを入れるか
	l_desc.NumDescriptors = a_descriptorCapacity;

	// ヒープをシェーダーから見えるようにするかどうか
	l_desc.Flags = m_isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// どのGPUノードで使用するかを指定する
	l_desc.NodeMask = GraphicsManager::GetVALDefaultGPUNodeMask();

	// ディスクリプタヒープを作成する関数
	// CreateDescriptorHeap(設定、
	//					    受け取りたいCOMインターフェース型のID、
	//					    作成結果のポインタを書き込むアドレス););

	auto l_hr = l_device->CreateDescriptorHeap(&l_desc, IID_PPV_ARGS(m_descriptorHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタを何個確保したか保存
	m_descriptorCapacity = a_descriptorCapacity;

	// ディスクリプタ1個分進めるのに必要なサイズを取得する
	// これを使ってディスクリプタハンドルの位置を計算する
	m_descriptorSize = l_device->GetDescriptorHandleIncrementSize(k_createDescriptorHeapType);

	// ヒープ先頭のCPUハンドルを取得する
	// CPUでCreateRenderTargetViewする際などに使う
	m_cpuStart = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// ShaderVisibleのヒープだけGPU側の先頭ハンドルを持てる
	if (m_isShaderVisible)
	{
		// GPUが参照するディスクリプタテーブルの開始位置
		m_gpuStart = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
	else
	{
		// ShaderVisibleではないヒープはGPUハンドルを使えないので無効値を入れておく
		m_gpuStart.ptr = k_invalidDescriptorPTR;
	}

	return true;	
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchVALCPUHandle(const UINT a_index) const
{
	if (!m_descriptorHeap)
	{
		assert(false && "ディスクリプタヒープが未作成でCPUハンドル取得ができません。");
		return {};
	}

	if (a_index >= m_descriptorCapacity)
	{
		assert(false && "ディスクリプタヒープのCPUハンドル取得に失敗、確保数を超えています。");
		return {};
	}

	// 先頭CPUハンドルを基準にする
	auto l_handle = m_cpuStart;

	// a_index個分先に進めて、目的のディスクリプタ位置を計算する
	l_handle.ptr += static_cast<UINT64>(a_index) * static_cast<UINT64>(m_descriptorSize);

	return l_handle;
}