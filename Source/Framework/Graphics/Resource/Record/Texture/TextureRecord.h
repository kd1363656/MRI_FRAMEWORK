#pragma once

namespace FWK::Graphics
{
	class TextureRecord final : public AssetRecordBase
	{
	public:

		 TextureRecord() = default;
		~TextureRecord() = default;

		TextureRecord(const TextureRecord&)			  = delete;
		TextureRecord(	    TextureRecord&&) noexcept = default;

		TextureRecord& operator=(const TextureRecord&)			 = delete;
		TextureRecord& operator=(	   TextureRecord&&) noexcept = default;

		bool PushDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue) override;

		void SetGPUResource(Struct::GPUResource&& a_set) { m_gpuResource = std::move(a_set); }

		void SetCurrentState(const D3D12_RESOURCE_STATES a_set) { m_currentState = a_set; }

		void SetSRVStorageID(const TypeAlias::StorageID a_set) { m_srvStorageID = a_set; }

		const Struct::GPUResource& GetREFGPUResource() const { return m_gpuResource; }

		D3D12_RESOURCE_STATES GetVALCurrentState() const { return m_currentState; }

		TypeAlias::StorageID GetVALSRVStorageID() const { return m_srvStorageID; }

	private:

		Struct::GPUResource m_gpuResource = {};
		
		D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;

		TypeAlias::StorageID m_srvStorageID = Constant::k_invalidStorageID;
	};
}