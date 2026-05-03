#pragma once

namespace FWK::Struct
{
	struct TextureRecord final
	{
		TypeAlias::ComPtr<ID3D12Resource2>     m_textureResource = nullptr;
		TypeAlias::ComPtr<D3D12MA::Allocation> m_allocation      = nullptr;

		D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;

		UINT m_srvIndex       = Constant::k_invalidDescriptorHeapIndex;
		UINT m_referenceCount = Constant::k_emptyTextureReferenceCount;

		TypeAlias::TextureID m_textureID = Constant::k_invalidTextureID;
	};

	struct TextureBatchUploadRecord
	{
		Struct::TextureRecord	    m_textureRecord       = {};
		Struct::TextureUploadRecord m_textureUploadRecord = {};
	};
}