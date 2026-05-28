#pragma once

namespace FWK::Graphics
{
	class StaticModelRecord final : public AssetRecordBase
	{
	public:

		 StaticModelRecord()		  = default;
		~StaticModelRecord() override = default;
		
		StaticModelRecord(const StaticModelRecord&)			  = delete;
		StaticModelRecord(	    StaticModelRecord&&) noexcept = default;

		StaticModelRecord& operator=(const StaticModelRecord&)			 = delete;
		StaticModelRecord& operator=(	   StaticModelRecord&&) noexcept = default;

		bool PushDeferredRelease(DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, const UINT64& a_retiredFenceValue) override;

		void SetModelData(Struct::ModelData&& a_modelData) { a_modelData = std::move(a_modelData); }

		Struct::ModelData& GetREFModelData() { return m_modelData; }

		const Struct::ModelData& GetREFModelData() const { return m_modelData; }

	private:

		bool IsValidStructuredBufferResource(const Struct::StructuredBufferResource& a_structuredBufferResource) const;

		bool PushStructuredBufferResource(DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, Struct::StructuredBufferResource& a_strcturedBufferResource, const UINT64& a_retiredFenceValue);

		Struct::ModelData m_modelData = {};
	};
}