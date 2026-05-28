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

		bool PushDeferredRelease(DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, const UINT64& a_retiredFenceValue) override;


	private:

	};
}