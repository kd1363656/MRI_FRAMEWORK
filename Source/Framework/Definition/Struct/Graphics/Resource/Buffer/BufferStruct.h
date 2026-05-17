#pragma once

namespace FWK::Struct
{
	struct StructuredBufferResource final
	{
		// StructuredBufferとして扱うGPUリソースとD3D12MA側Allocation
		GPUResource m_bufferGPUResource = {};

		// StructuredBuffer用SRVのStorageID
		TypeAlias::StorageID m_srvStorageID = Constant::k_invalidStorageID;
	};
}