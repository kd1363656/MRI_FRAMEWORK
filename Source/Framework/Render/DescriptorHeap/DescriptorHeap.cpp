#include "DescriptorHeap.h"

FWK::Render::DescriptorHeap::DescriptorHeap(const Hardware& a_hardware, const D3D12_DESCRIPTOR_HEAP_TYPE a_createDescriptorHeapType) : 
	k_createDescriptorHeapType(a_createDescriptorHeapType),
	k_hardware                (a_hardware),
	m_descriptorHeap          (nullptr),
	m_cpuStart                ({}),
	m_gpuStart                ({}),
	m_descriptorSize          (0U),
	m_capacity                (0U)
{}
FWK::Render::DescriptorHeap::~DescriptorHeap() = default;

void FWK::Render::DescriptorHeap::Init()
{
	m_descriptorHeap.Reset();

	m_cpuStart = {};
	m_gpuStart = {};

	m_descriptorSize = 0U;
	m_capacity = 0U;
}
bool FWK::Render::DescriptorHeap::Create(const std::uint32_t a_numDescriptors, const bool a_shaderVisible)
{
	const auto& l_device = k_hardware.GetDevice();

	// デバイスが作成されていなければreturn
	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタの数を格納
	m_capacity = a_numDescriptors;

	D3D12_DESCRIPTOR_HEAP_DESC l_desc = {};

	l_desc.Type           = k_createDescriptorHeapType;																	   // ディスクリプタの種類
	l_desc.NumDescriptors = a_numDescriptors;																			   // ディスクリプタの数
	l_desc.Flags          = a_shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // GPUのシェーダーから直接参照可能かどうか
	l_desc.NodeMask       = k_hardware.GetGPUNodeMask();															       // どのGPUノード用のディスクリプタヒープかどうか

	auto l_hr = l_device->CreateDescriptorHeap(&l_desc, IID_PPV_ARGS(m_descriptorHeap.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "ディスクリプタヒープの作成に失敗しました。");
		return false;
	}

	// ディスクリプタの種類から加算サイズを取得(ポインタの位置をずらすのに使う)
	m_descriptorSize = l_device->GetDescriptorHandleIncrementSize(k_createDescriptorHeapType);

	// CPU記述子ハンドルの開始位置を格納
	m_cpuStart = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// もしシェーダー可視性があるならGPU記述子ハンドルの開始位置を格納
	if (a_shaderVisible)
	{
		m_gpuStart = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
	else
	{
		m_gpuStart.ptr = k_invalidDescriptorPtr; // 安全のため明示
	}

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE FWK::Render::DescriptorHeap::GetCPUHandle(const std::uint32_t a_index) const
{
	// 容量を超えていたらassert
	if (a_index >= m_capacity)
	{
		assert(false && "ディスクリプタヒープのCPUハンドルの取得に失敗、確保したディスクリプタの数を超えています。");
		return {};
	}

	// ヒープの種類のサイズ分加算してポインタを進める
	D3D12_CPU_DESCRIPTOR_HANDLE l_handle = m_cpuStart;

	l_handle.ptr += static_cast<SIZE_T>(a_index) * m_descriptorSize;

	return l_handle;
}
D3D12_GPU_DESCRIPTOR_HANDLE FWK::Render::DescriptorHeap::GetGPUHandle(const std::uint32_t a_index) const
{
	// ポインタのスタート位置が0ならヒープ開始位置がないためreturn
	if (m_gpuStart.ptr == k_invalidDescriptorPtr)
	{
		assert(false && "ディスクリプタヒープのGPUハンドルの取得に失敗、GPU側からアクセスできるディスクリプタヒープではありません。");
		return {};
	}

	// 容量を超えていたらreturn
	if (a_index >= m_capacity)
	{
		assert(false && "ディスクリプタヒープのGPUハンドルの取得に失敗、確保したディスクリプタの数を超えています。");
		return {};
	}

	// ヒープの種類のサイズ分加算してポインタを進める
	D3D12_GPU_DESCRIPTOR_HANDLE l_handle = m_gpuStart;

	l_handle.ptr += static_cast<SIZE_T>(a_index) * m_descriptorSize;

	return l_handle;
}