#pragma once

namespace FWK::Graphics
{
	// SRVのインデックスなどを扱うためコピー禁止
	class AssetRecordBase
	{
	public:

				 AssetRecordBase() = default;
		virtual ~AssetRecordBase() = default;

		AssetRecordBase(const AssetRecordBase&)			  = delete;
		AssetRecordBase(	  AssetRecordBase&&) noexcept = default;

		AssetRecordBase& operator=(const AssetRecordBase&)			 = delete;
		AssetRecordBase& operator=(		 AssetRecordBase&&) noexcept = default;

		void AddReferenceCount();

		bool SubReferenceCount();

		bool IsUnused() const;

		void InvalidateStorageID();

		virtual bool PushDeferredRelease(DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, const UINT64& a_retiredFenceValue) = 0;

		void SetStorageID(const TypeAlias::StorageID& a_set) { m_storageID = a_set; }

		TypeAlias::StorageID GetVALStorageID() const { return m_storageID; }

		std::uint32_t GetVALReferenceCount() const { return m_referenceCount; }

	private:

		TypeAlias::StorageID m_storageID = Constant::k_invalidStorageID;

		std::uint32_t m_referenceCount = Constant::k_emptyAssetReferenceCount;
	};
}