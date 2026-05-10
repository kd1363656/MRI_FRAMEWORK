#pragma once

namespace FWK::Struct
{
	struct BackBuffer final
	{
		TypeAlias::ComPtr<ID3D12Resource2> m_backBufferResource = nullptr;

		TypeAlias::StorageID m_rtvStorageID = Constant::k_invalidStorageID;
	};
}