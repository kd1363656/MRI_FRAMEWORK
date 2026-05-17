#pragma once

namespace FWK::Struct
{
	struct StructuredBufferResource final
	{
		// StructuredBufferとして扱うGPUリソース
		GPUResource m_bufferResource = {};

		// StructuredBuffer用SRVのStorageID
		TypeAlias::StorageID m_srvStorageID = Constant::k_invalidStorageID;
	};
}