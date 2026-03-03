#include "DescriptorHeapBase.h"

FWK::Graphics::DescriptorHeapBase::DescriptorHeapBase(const Device& a_device, const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptroHeapType, const bool a_isShaderVisible) : 
	k_device				  (a_device),
	k_createDescriptorHeapType(a_createDescriptroHeapType),
	k_isShaderVisible         (a_isShaderVisible),
	m_descriptorHeap          (nullptr),
	m_cpuStart				  (),
	m_gpuStart				  (),
	m_capacity				  (0U),
	m_descriptorSize		  (0U)
{}
FWK::Graphics::DescriptorHeapBase::~DescriptorHeapBase() = default;

void FWK::Graphics::DescriptorHeapBase::Init()
{
	m_descriptorHeap.Reset();

	m_cpuStart     = D3D12_CPU_DESCRIPTOR_HANDLE();
	m_gpuStart     = D3D12_GPU_DESCRIPTOR_HANDLE();
	m_gpuStart.ptr = k_invalidDescriptorPtr;

	m_capacity       = 0U;
	m_descriptorSize = 0U;
}
bool FWK::Graphics::DescriptorHeapBase::Create(const UINT a_capacity)
{
	const auto& l_device = k_device.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	if (a_capacity == 0U)
	{
		assert(false && "作成するディスクリプタ数が0です。");
		return false;
	}

	m_capacity = a_capacity;

	D3D12_DESCRIPTOR_HEAP_DESC l_desc = {};

	l_desc.Type           = k_createDescriptorHeapType;																		 // 作成するディスクリプタの種類
	l_desc.NumDescriptors = a_capacity;																						 // ディスクリプタヒープの容量
	l_desc.Flags          = k_isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // シェーダー側から見て見えるかどうか
	l_desc.NodeMask       = k_device.GetGPUNodeMask();																		 // 単一グラフィックカードかどうか

	auto l_hr = l_device->CreateDescriptorHeap(&l_desc, IID_PPV_ARGS(m_descriptorHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタ1個分の増分サイズとCPU側ディスクリプタヒープの開始ハンドルを取得
	m_descriptorSize = l_device->GetDescriptorHandleIncrementSize(k_createDescriptorHeapType);
	m_cpuStart       = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// GPUハンドルはShaderVisibleヒープでのみ有効
	// 未ShaderVisibleヒープでは無効値を保持する
	if (k_isShaderVisible)
	{
		m_gpuStart = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
	else
	{
		m_gpuStart.ptr = k_invalidDescriptorPtr;
	}

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchCPUHandle(const UINT a_index) const
{
	if (!m_descriptorHeap)
	{
		assert(false && "ディスクリプタヒープが未作成でCPUハンドル取得ができません。");
		return {};
	}

	if (a_index >= m_capacity)
	{
		assert(false && "ディスクリプタヒープのCPUハンドル取得に失敗、確保数を超えています。");
		return {};
	}

	D3D12_CPU_DESCRIPTOR_HANDLE l_handle = m_cpuStart;
	l_handle.ptr += static_cast<SIZE_T>(a_index) * m_descriptorSize;

	return l_handle;
}
D3D12_GPU_DESCRIPTOR_HANDLE FWK::Graphics::DescriptorHeapBase::FetchGPUHandle(const UINT a_index) const
{
	if (!m_descriptorHeap)
	{
		assert(false && "ディスクリプタヒープが未作成でGPUハンドル取得ができません。");
		return {};
	}

	if (!k_isShaderVisible || m_gpuStart.ptr == k_invalidDescriptorPtr)
	{
		assert(false && "GPUハンドル取得に失敗。ShaderVisibleではありません。");
		return {};
	}

	if (a_index >= m_capacity)
	{
		assert(false && "ディスクリプタヒープのGPUハンドル取得に失敗、確保数を超えています。");
		return {};
	}

	D3D12_GPU_DESCRIPTOR_HANDLE l_handle = m_gpuStart;
	l_handle.ptr += static_cast<SIZE_T>(a_index) * m_descriptorSize;

	return l_handle;
}