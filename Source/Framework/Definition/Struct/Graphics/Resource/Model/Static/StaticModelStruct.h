#pragma once

namespace FWK::Struct
{
	struct StaticModelRecord final : public AssetRecordBase
	{
		 StaticModelRecord() = default;
		~StaticModelRecord() = default;

		StaticModelRecord(const StaticModelRecord&)           = delete;
		StaticModelRecord(      StaticModelRecord&&) noexcept = default;

		StaticModelRecord& operator=(const StaticModelRecord&)            = delete;
		StaticModelRecord& operator=(      StaticModelRecord&&) noexcept = default;

		ModelData m_modelData = {};
	};
}