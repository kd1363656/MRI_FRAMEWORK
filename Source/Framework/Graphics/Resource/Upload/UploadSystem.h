#pragma once

namespace FWK::Graphics
{
	class UploadSystem final
	{
	public:

		 UploadSystem() = default;
		~UploadSystem() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    (const Device& a_device);

		bool SubmitTextureCopyBatchAndWait(const std::vector<Struct::TextureUploadRecord>& a_textureUploadRecordList);

		nlohmann::json Serialize() const;

		const auto& GetREFCopyCommandAllocatorList() const { return m_copyCommandAllocatorList; }

		auto& GetMutableREFCopyCommandAllocatorList() { return m_copyCommandAllocatorList; }

	private:

		void RecordTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer, const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList) const;

		bool ValidateTextureUploadRecord(const Struct::TextureUploadRecord& a_textureUploadRecord) const;

		CopyCommandAllocator* FetchMutablePTRCopyCommandAllocator();

		static constexpr UINT k_textureCopyDestinationX = 0U;
		static constexpr UINT k_textureCopyDestinationY = 0U;
		static constexpr UINT k_textureCopyDestinationZ = 0U;

		static constexpr std::size_t k_initialCurrentCopyCommandAllocatorIndex = 0ULL;
		static constexpr std::size_t k_copyCommandAllocatorIndexIncrement      = 1ULL;
		
		CopyCommandQueue m_copyCommandQueue = {};
		CopyCommandList  m_copyCommandList  = {};

		JsonConverter::UploadSystemJsonConverter m_uploadSystemJsonConverter = {};

		std::vector<CopyCommandAllocator> m_copyCommandAllocatorList = {};
		
		std::size_t m_currentCopyCommandAllocatorIndex = k_initialCurrentCopyCommandAllocatorIndex;
	};
}