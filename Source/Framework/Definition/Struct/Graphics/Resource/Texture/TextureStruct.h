#pragma once

namespace FWK::Struct
{
	struct TextureRecord final
	{
		TypeAlias::ComPtr<ID3D12Resource2>     m_textureResource = nullptr;
		TypeAlias::ComPtr<D3D12MA::Allocation> m_allocation      = nullptr;

		UINT m_srvIndex = Constant::k_invalidDescriptorHeapIndex;

		TypeAlias::TextureID m_textureID = Constant::k_invalidTextureID;

		std::wstring m_filePath = {};
	};
}