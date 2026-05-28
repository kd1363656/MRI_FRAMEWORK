#pragma once

namespace FWK::Struct
{
	struct DeferredResourceReleaseRecordBase
	{
		UINT64 m_retiredFenceValue = Constant::k_unusedFenceValue;
	};

	struct GPUResourceReleaseRecord final : public DeferredResourceReleaseRecordBase
	{
		Struct::GPUResource m_gpuResource = {};
	};

	struct DescriptorIndexReleaseRecord final : public DeferredResourceReleaseRecordBase
	{
		TypeAlias::StorageID m_storageID = {};
	};
}