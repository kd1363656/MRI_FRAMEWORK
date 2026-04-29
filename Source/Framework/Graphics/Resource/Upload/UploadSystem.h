#pragma once

namespace FWK::Graphics
{
	class UploadSystem final
	{
	private:

		struct PendingUploadBuffer final
		{
			UploadBuffer m_uploadBuffer = {};

			UINT64 m_fenceValue = Constant::k_unusedFenceValue;
		};

	public:

		 UploadSystem() = default;
		~UploadSystem() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    (const Device& a_device);

		void UpdateCompletedUploads();

		nlohmann::json Serialize() const;

		bool IsUploadCompleted(const UINT64& a_fenceValue) const;

		bool SubmitTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>&              a_textureResource,
							   const UploadBuffer&						              a_uploadBuffer,
							   const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList,
									 UINT64&										  a_submittedFenceValue);

		const auto& GetREFCopyCommandAllocatorList() const { return m_copyCommandAllocatorList; }

		auto& GetMutableREFCopyCommandAllocatorList() { return m_copyCommandAllocatorList; }

	private:

		void RecordTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer, const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList) const;

		CopyCommandAllocator* FetchMutablePTRCopyCommandAllocator();

		static constexpr std::size_t k_pendingUploadBufferBeginIndex		             = 0ULL;
		static constexpr std::size_t k_initialCurrentCopyCommandAllocatorIndex           = 0ULL;
		static constexpr std::size_t k_pendingUploadBufferListSizeToLastIndexSubtraction = 1ULL;
		static constexpr std::size_t k_nextCopyCommandAllocatorIndexStep				 = 1ULL;
		static constexpr std::size_t k_defaultCopyCommandAllocatorListSize			     = 3ULL;

		CopyCommandQueue m_copyCommandQueue = {};
		CopyCommandList  m_copyCommandList  = {};

		JsonConverter::UploadSystemJsonConverter m_uploadSystemJsonConverter = {};

		std::vector<CopyCommandAllocator> m_copyCommandAllocatorList = {};
		std::vector<PendingUploadBuffer>  m_pendingUploadBufferList  = {};

		std::size_t m_currentCopyCommandAllocatorIndex = k_initialCurrentCopyCommandAllocatorIndex;
	};
}