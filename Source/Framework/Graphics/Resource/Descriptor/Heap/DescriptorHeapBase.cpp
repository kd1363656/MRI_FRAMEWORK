#include "DescriptorHeapBase.h"

FWK::Graphics::DescriptorHeapBase::DescriptorHeapBase(const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType, const bool a_isUseCPUOnly, const bool a_isUseShaderVisible) :
	k_createDescriptorHeapType(a_createDescriptorHeapType),

	k_isUseCPUOnly      (a_isUseCPUOnly),
	k_isUseShaderVisible(a_isUseShaderVisible),

	m_cpuOnlyDescriptorHeapRecord      (nullptr),
	m_shaderVisibleDescriptorHeapRecord(nullptr),

	m_descriptorCapacity(0U),
	m_descriptorSize    (0U)
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
	if (a_descriptorCapacity == Constant::k_invalidDescriptorCapacity)
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

	// ディスクリプタを何個確保するかを保存
	m_descriptorCapacity = a_descriptorCapacity;

	// ディスクリプタ1個分進めるのに必要なサイズを取得する
	// これを使ってディスクリプタハンドルの位置を計算する
	m_descriptorSize = l_device->GetDescriptorHandleIncrementSize(k_createDescriptorHeapType);

	// CPUOnlyのディスクリプタヒープを使用する場合のみ作成する
	if(k_isUseCPUOnly)
	{
		m_cpuOnlyDescriptorHeapRecord = std::make_shared<DescriptorHeapRecord>();

		if (!CreateDescriptorHeapRecord(D3D12_DESCRIPTOR_HEAP_FLAG_NONE, a_device, *m_cpuOnlyDescriptorHeapRecord))
		{
			assert(false && "CPUOnly用ディスクリプタヒープの作成に失敗しました。");
			return false;
		}
	}

	// ShaderVisibleのディスクリプタヒープを使用する場合のみ作成する
	if (k_isUseShaderVisible)
	{
		m_shaderVisibleDescriptorHeapRecord = std::make_shared<DescriptorHeapRecord>();

		if (!CreateDescriptorHeapRecord(D3D12_DESCRIPTOR_HEAP_FLAG_NONE, a_device, *m_shaderVisibleDescriptorHeapRecord))
		{
			assert(false && "ShaderVisible用ディスクリプタヒープの作成に失敗しました。");
			return false;
		}
	}

	return true;	
}

bool FWK::Graphics::DescriptorHeapBase::CreateDescriptorHeapRecord(const D3D12_DESCRIPTOR_HEAP_FLAGS a_descriptorHeapFlag, const Device& a_device, DescriptorHeapRecord& a_descriptorHeapRecord)
{
	return false;
}
bool FWK::Graphics::DescriptorHeapBase::CreateDescriptorHeapRecord(const D3D12_DESCRIPTOR_HEAP_FLAGS a_descriptorHeapFlag, const Device& a_device, const bool a_shouldCreate, std::shared_ptr<DescriptorHeapRecord>& a_descriptorHeapRecord)
{
	return false;
}