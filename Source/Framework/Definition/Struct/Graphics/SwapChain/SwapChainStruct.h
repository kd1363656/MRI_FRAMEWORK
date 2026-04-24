#pragma once

namespace FWK::Struct
{
	struct BackBuffer final
	{
		TypeAlias::ComPtr<ID3D12Resource2> m_backBufferResourceREF = nullptr;

		UINT m_rtvIndex = Constant::k_invalidDescriptorHeapIndex;
	};
}