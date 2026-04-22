#pragma once

namespace FWK::Struct
{
	struct BackBuffer
	{
		UINT m_rtvIndex = Constant::k_invalidDescriptorHeapIndex;

		TypeAlias::ComPtr<ID3D12Resource2> m_resource = nullptr;
	};
}