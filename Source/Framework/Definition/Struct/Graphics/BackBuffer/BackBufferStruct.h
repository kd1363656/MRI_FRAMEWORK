#pragma once

namespace FWK::Struct
{
	struct BackBuffer
	{
		TypeAlias::ComPtr<ID3D12Resource2> m_resource = nullptr;

		UINT m_rtvIndex = Constant::k_invalidDescriptorHeapIndex;
	};
}