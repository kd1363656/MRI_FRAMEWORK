#pragma once

namespace FWK::Graphics
{
	class StaticModelRecord;
}

namespace FWK::Struct
{
	struct StaticModelRecord final : public AssetRecordBase
	{
		 StaticModelRecord() = default;
		~StaticModelRecord() = default;

		StaticModelRecord(const StaticModelRecord&)           = delete;
		StaticModelRecord(      StaticModelRecord&&) noexcept = default;

		StaticModelRecord& operator=(const StaticModelRecord&)           = delete;
		StaticModelRecord& operator=(      StaticModelRecord&&) noexcept = default;

		ModelData m_modelData = {};
	};

	struct StaticModelResult final
	{
		std::weak_ptr<Graphics::StaticModelRecord> m_staticModelRecord = {};

		TypeAlias::StorageID m_storageID = Constant::k_invalidStorageID;
	};

	struct StaticModelBatchUploadRecord final
	{
		// StaticModelStorageへ登録するStaticModelRecord
		std::shared_ptr<Struct::StaticModelRecord> m_staticModelRecord = nullptr;

		// StaticModelのBufferResourceへコピーするためのUploadCommand一覧
		std::vector<Struct::BufferUploadCommand> m_bufferUploadCommandList = {};
	};
}