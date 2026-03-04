#include "RootSignature.h"

FWK::Graphics::RootSignature::RootSignature(const Device& a_device) : 
	k_device       (a_device),
	m_rootSignature(nullptr)
{}
FWK::Graphics::RootSignature::~RootSignature() = default;

void FWK::Graphics::RootSignature::Init()
{
	m_rootSignature.Reset();
}
bool FWK::Graphics::RootSignature::Create()
{
	const auto& l_device = k_device.GetDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずRootSignatureを作成できませんでした。");
		return false;
	}

	D3D12_ROOT_SIGNATURE_DESC l_desc = {};

	// 今回は最小構成(リソースなし)
	l_desc.NumParameters     = 0U;
	l_desc.pParameters       = nullptr;
	l_desc.NumStaticSamplers = 0U;
	l_desc.pStaticSamplers   = nullptr;

	// "MeshShader"前提で安全なフラグ
	l_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<ID3DBlob> l_serializedBlob = nullptr;
	ComPtr<ID3DBlob> l_errorBlob      = nullptr;

	auto l_hr = D3D12SerializeRootSignature(&l_desc, 
										    D3D_ROOT_SIGNATURE_VERSION_1, 
											l_serializedBlob.ReleaseAndGetAddressOf(),
											l_errorBlob.ReleaseAndGetAddressOf     ());

	if (FAILED(l_hr))
	{
		if (l_errorBlob)
		{
			OutputDebugStringA((char*)l_errorBlob->GetBufferPointer());
		}

		assert(false && "ルートシグネチャのシリアライズに失敗しました。");
		return false;
	}

	l_hr = l_device->CreateRootSignature(k_device.GetGPUNodeMask           (),
										 l_serializedBlob->GetBufferPointer(),
										 l_serializedBlob->GetBufferSize   (),
										 IID_PPV_ARGS					   (m_rootSignature.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "ルートシグネチャの作成に失敗しました。");
		return false;
	}

	return true;
}