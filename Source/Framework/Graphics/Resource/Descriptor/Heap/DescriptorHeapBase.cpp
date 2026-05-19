#include "DescriptorHeapBase.h"

FWK::Graphics::DescriptorHeapBase::DescriptorHeapBase(const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType, const bool a_isUseCPUOnly, const bool a_isUseShaderVisible) :
	k_createDescriptorHeapType(a_createDescriptorHeapType),

	k_isUseCPUOnly      (a_isUseCPUOnly),
	k_isUseShaderVisible(a_isUseShaderVisible),

	m_cpuOnlyDescriptorHeapRecord      (nullptr),
	m_shaderVisibleDescriptorHeapRecord(nullptr),

	m_descriptorStorageIDCapacity(Constant::k_invalidStorageIDCapacity),
	m_descriptorSize			 (k_uninitializedDescriptorSize)
{}
FWK::Graphics::DescriptorHeapBase::~DescriptorHeapBase() = default;

bool FWK::Graphics::DescriptorHeapBase::Create(const Device& a_device, const TypeAlias::StorageID a_storageIDCapacity)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタ数0のヒープは意味がないので失敗扱い
	if (a_storageIDCapacity == Constant::k_invalidStorageIDCapacity)
	{
		assert(false && "作成するディスクリプタ数が0です。");
		return false;
	}

	// CPUOnlyかShaderVisibleのどちらも使用しない場合作る必要のないクラスになってしまうのでreturn
	if (!k_isUseCPUOnly && !k_isUseShaderVisible)
	{
		assert(false && "CPUOnlyとShaderVisibleのどちらのディスクリプタヒープも使用しない設定になっています。");
		return false;
	}

	// ShaderVisibleにできるのはCBV_SRV_UAVとSAMPLERだけ
	if (k_isUseShaderVisible												 &&
		k_createDescriptorHeapType != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV &&
		k_createDescriptorHeapType != D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		assert(false && "ShaderVisibleにできない種類のディスクリプタヒープです。");
		return false;
	}

	// ディスクリプタを何個確保するかを保存
	m_descriptorStorageIDCapacity = a_storageIDCapacity;

	// ディスクリプタ1個分進めるのに必要なサイズを取得する
	// これを使ってディスクリプタハンドルの位置を計算する
	m_descriptorSize = l_device->GetDescriptorHandleIncrementSize(k_createDescriptorHeapType);

	// CPUOnlyのディスクリプタヒープを使用する場合のみ作成する
	if (!CreateDescriptorHeapRecordIfNeeded(a_device,
											D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
										    k_isUseCPUOnly,
										    m_cpuOnlyDescriptorHeapRecord))
	{
		assert(false && "CPUOnly用ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ShaderVisibleのディスクリプタヒープを使用する場合のみ作成する
	if (!CreateDescriptorHeapRecordIfNeeded(a_device, 
											D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
										    k_isUseShaderVisible,
										    m_shaderVisibleDescriptorHeapRecord))
	{
		assert(false && "ShaderVisible用ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::DescriptorHeapBase::CopyCPUOnlyDescriptorToShaderVisibleDescriptor(const TypeAlias::StorageID a_storageID, const Device& a_device) const
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、ShaderVisible用ディスクリプタコピーに失敗しました。");
		return false;
	}

	if (!m_cpuOnlyDescriptorHeapRecord)
	{
		assert(false && "CPUOnly用ディスクリプタヒープが未作成で、ShaderVisible用ディスクリプタコピーに失敗しました。");
		return false;
	}

	if (!m_shaderVisibleDescriptorHeapRecord)
	{
		assert(false && "ShaderVisible用ディスクリプタヒープが未作成で、ShaderVisible用ディスクリプタコピーに失敗しました。");
		return false;
	}

	const auto& l_srcCPUHandle = FetchVALCPUOnlyCPUHandle      (a_storageID);
	const auto& l_dstCPUHandle = FetchVALShaderVisibleCPUHandle(a_storageID);

	// CPUOnly側に作成したディスクリプタをShaderVisible側へコピーする
	// CopyDescriptorsSimple(コピーするディスクリプタ数、
	//						 コピー先のCPUディスクリプタハンドル、
	//						 コピー元のCPUディスクリプタハンドル、
	//						 コピーするディスクリプタヒープの種類);

	l_device->CopyDescriptorsSimple(k_copyDescriptorCount,
									l_dstCPUHandle,
									l_srcCPUHandle,
									k_createDescriptorHeapType);

	return true;
}

FWK::TypeAlias::ComPtr<ID3D12DescriptorHeap> FWK::Graphics::DescriptorHeapBase::FetchPTRShaderVisibleDescriptorHeap() const
{
	if (!m_shaderVisibleDescriptorHeapRecord)
	{
		assert(false && "ShaderVisible用ディスクリプタヒープが未作成でディスクリプタヒープ取得ができませんでした。");
		return nullptr;
	}

	return m_shaderVisibleDescriptorHeapRecord->m_descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchVALCPUOnlyCPUHandle(const TypeAlias::StorageID a_storageID) const
{
	if (!m_cpuOnlyDescriptorHeapRecord)
	{
		assert(false && "CPUOnly用ディスクリプタヒープが未作成でCPUハンドル取得ができません。");
		return {};
	}

	return FetchVALCPUHandle(a_storageID, *m_cpuOnlyDescriptorHeapRecord);
}
D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchVALShaderVisibleCPUHandle(const TypeAlias::StorageID a_storageID) const
{
	if (!m_shaderVisibleDescriptorHeapRecord)
	{
		assert(false && "ShaderVisible用ディスクリプタヒープが未作成でCPUハンドル取得ができません。");
		return {};
	}

	return FetchVALCPUHandle(a_storageID, *m_shaderVisibleDescriptorHeapRecord);
}

D3D12_GPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchVALShaderVisibleGPUHandle(const TypeAlias::StorageID a_storageID) const
{
	if (!m_shaderVisibleDescriptorHeapRecord)
	{
		assert(false && "ShaderVisible用ディスクリプタヒープが無効でGPUハンドル取得ができません。");
		return {};
	}

	return FetchVALGPUHandle(a_storageID, *m_shaderVisibleDescriptorHeapRecord);
}

bool FWK::Graphics::DescriptorHeapBase::CreateDescriptorHeapRecord(const Device& a_device, const D3D12_DESCRIPTOR_HEAP_FLAGS a_descriptorHeapFlag, DescriptorHeapRecord& a_descriptorHeapRecord) const
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタヒープ作成設定を入れる構造体
	D3D12_DESCRIPTOR_HEAP_DESC l_desc = {};

	// このヒープの種類を設定する(RTV / DSV / CBV_SRV_UAV / SAMPLERの内どれか)
	l_desc.Type = k_createDescriptorHeapType;

	// このヒープに何個ディスクリプタを入れるか
	l_desc.NumDescriptors = m_descriptorStorageIDCapacity;

	// ヒープをシェーダーから見えるようにするかどうか
	l_desc.Flags = a_descriptorHeapFlag;

	// どのGPUノードで使用するかを指定する
	l_desc.NodeMask = Constant::k_defaultGPUNodeMask;

	// ディスクリプタヒープを作成する関数
	// CreateDescriptorHeap(設定、
	//					    受け取りたいCOMインターフェース型のID、
	//					    作成結果のポインタを書き込むアドレス);

	auto l_hr = l_device->CreateDescriptorHeap(&l_desc, IID_PPV_ARGS(a_descriptorHeapRecord.m_descriptorHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ヒープ先頭のCPUハンドルを取得する
	// RTV作成、SRV作成、ディスクリプタコピー元/コピー先の指定などに使う
	
	// ShaderVisibleでもそうでないCPUOnlyでも使用するため格納する
	a_descriptorHeapRecord.m_cpuStart = a_descriptorHeapRecord.m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// ShaderVisibleのヒープだけGPU側の先頭ハンドルを持てる
	if (a_descriptorHeapFlag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		// GPUが参照するディスクリプタテーブルの開始位置を取得する
		a_descriptorHeapRecord.m_gpuStart = a_descriptorHeapRecord.m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
	else 
	{
		// ShaderVisibleではないヒープはGPUハンドルを使えないので無効値を入れておく
		a_descriptorHeapRecord.m_gpuStart.ptr = k_invalidGPUDescriptorHandlePTR;
	}

	return true;
}
bool FWK::Graphics::DescriptorHeapBase::CreateDescriptorHeapRecordIfNeeded(const Device&								a_device, 
																		   const D3D12_DESCRIPTOR_HEAP_FLAGS            a_descriptorHeapFlag,
																		   const bool						            a_shouldCreate,
																			     std::shared_ptr<DescriptorHeapRecord>& a_descriptorHeapRecord) const
{
	// 作る必要がなければ成功扱いでreturn
	if (!a_shouldCreate) { return true; }

	a_descriptorHeapRecord = std::make_shared<DescriptorHeapRecord>();

	if (!a_descriptorHeapRecord)
	{
		assert(false && "ディスクリプタヒープ情報の作成に失敗しました。");
		return false;
	}

	if (!CreateDescriptorHeapRecord(a_device, a_descriptorHeapFlag, *a_descriptorHeapRecord))
	{
		assert(false && "ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchVALCPUHandle(const TypeAlias::StorageID a_storageID, const DescriptorHeapRecord& a_descriptorHeapRecord) const
{
	if (!a_descriptorHeapRecord.m_descriptorHeap)
	{
		assert(false && "ディスクリプタヒープが未作成でCPUハンドル取得ができません。");
		return {};
	}

	if (a_storageID >= m_descriptorStorageIDCapacity)
	{
		assert(false && "ディスクリプタヒープの確保上限数を超えておりディスクリプタヒープのCPUハンドル取得に失敗しました。");
		return {};
	}

	// 先頭CPUハンドルを基準にする
	auto l_handle = a_descriptorHeapRecord.m_cpuStart;

	// a_storageID個分先に進めて、目的のディスクリプタ位置を計算する
	l_handle.ptr += static_cast<UINT64>(a_storageID) * static_cast<UINT64>(m_descriptorSize);

	return l_handle;
}
D3D12_GPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchVALGPUHandle(const TypeAlias::StorageID a_storageID, const DescriptorHeapRecord& a_descriptorHeapRecord) const
{
	if (!a_descriptorHeapRecord.m_descriptorHeap)
	{
		assert(false && "ディスクリプタヒープが未作成でGPUハンドル取得ができません。");
		return {};
	}

	if (a_storageID >= m_descriptorStorageIDCapacity)
	{
		assert(false && "ディスクリプタヒープの確保上限数を超えておりディスクリプタヒープのGPUハンドル取得に失敗しました。");
		return {};
	}

	// 先頭GPUハンドルを基準にする
	auto l_handle = a_descriptorHeapRecord.m_gpuStart;

	// a_storageID個分先に進めて、目的のディスクリプタ位置を計算する
	l_handle.ptr += static_cast<UINT64>(a_storageID * m_descriptorSize);

	return l_handle;
}