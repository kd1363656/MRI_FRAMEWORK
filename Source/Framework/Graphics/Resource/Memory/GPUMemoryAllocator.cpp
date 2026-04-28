#include "GPUMemoryAllocator.h"

bool FWK::Graphics::GPUMemoryAllocator::Create(const Device& a_device)
{
    const auto& l_device = a_device.GetREFDevice();

    if (!l_device)
    {
        assert(false && "デバイスが作成されておらず、GPUメモリアロケータの作成に失敗しました。");
        return false;
    }

    const auto& l_adapter = a_device.GetREFAdapter();

    if (!l_adapter)
    {
        assert(false && "GPUアダプターが保存されておらず、GPUメモリアロケータの作成に失敗しました。");
        return false;
    }

    D3D12MA::ALLOCATOR_DESC l_allocatorDesc = {};

    // D3D12MA::ALLOCATOR_DESCについて
    // pDevice  : D3D12MAがリソース作成に使用するD3D12デバイス
    // pAdapter : D3D12MAがメモリ情報を取得に使用するGPUアダプター
    l_allocatorDesc.pDevice  = l_device.Get ();
    l_allocatorDesc.pAdapter = l_adapter.Get();

    // D3D12MA::CreateAllocator(アロケータ作成設定、
    //                          作成されたD3D12MAアロケータの受け取り先);
    const auto l_hr = D3D12MA::CreateAllocator(&l_allocatorDesc, m_allocator.ReleaseAndGetAddressOf());

    if (FAILED(l_hr))
    {
        assert(false && "D3D12MAアロケータの作成に失敗しました。");
        return false;
    }

    return true;
}

bool FWK::Graphics::GPUMemoryAllocator::CreateTextureResource(const D3D12_RESOURCE_DESC& a_resourceDesc, const D3D12_CLEAR_VALUE* a_clearValue, const D3D12_RESOURCE_STATES a_initialResourceState, TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, TypeAlias::ComPtr<D3D12MA::Allocation>& a_allocation) const
{
    a_textureResource.Reset();
    a_allocation.Reset     ();

    if (!m_allocator)
    {
        assert(false && "D3D12MAアロケータが作成されておらず、TextureResourceの作成に失敗しました。");
        return false;
    }

    D3D12MA::ALLOCATION_DESC l_allocationDesc = {};

    // D3D12MA::ALLOCATION_DESCについて
    // HeapType : リソースをどの種類のGPUメモリに置くか
    // TextureResourceはGPU側で使用する本番リソースなのでDEFAULTを指定する
    l_allocationDesc.HeapType = k_textureResourceHeapType;

    // D3D12MA::Allocator::CreateResource(割り当て設定、
    //                                    作成するリソース設定、
    //                                    作成直後のリソースステート、
    //                                    ClearValue(nullptrなら未使用),
    //                                    D3D12MA側Allocationの受取先、
    //                                    受け取りたいResourceインターフェース型ID、
    //                                    作成されたResourceの受け取り先);
    const auto l_hr = m_allocator->CreateResource(&l_allocationDesc,
                                                  &a_resourceDesc,
                                                  a_initialResourceState,
                                                  a_clearValue,
                                                  a_allocation.ReleaseAndGetAddressOf(),
                                                  IID_PPV_ARGS(a_textureResource.ReleaseAndGetAddressOf()));

    if (FAILED(l_hr))
    {
        assert(false && "D3D12MAによるTextureResourceの作成に失敗しました。");
        return false;
    }

    return true;
}