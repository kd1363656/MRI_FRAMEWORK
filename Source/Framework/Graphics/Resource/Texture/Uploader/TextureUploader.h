#pragma once

namespace FWK::Graphics
{
	class TextureUploader final
	{
	public:

		TextureUploader () = default;
		~TextureUploader() = default;

		void Init();

		bool StageTexture(const DirectX::TexMetadata& a_texMetadata, const DirectX::ScratchImage& a_image, CommonStruct::TextureRecord& a_inOutRecord);

		void FlushUploads(std::vector<CommonStruct::TextureRecord>& a_outUploadedRecordList);

	private:

		bool AllocateDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc, ComPtr<ID3D12Heap>& a_outHeap, ComPtr<ID3D12Resource2>& a_outResource) const;

		bool AllocateUploadMemory(const UINT64& a_size, const UINT64& a_alignment, CommonStruct::UploadAllocation& a_outAllocation);

		void GarbageCollectUploadPage();

		bool CreateUploadPage(const UINT64& a_pageSize, CommonStruct::UploadPage& a_outPage) const;

		static constexpr UINT64 k_oneMegaByte = 1024ULL * 1024ULL;

		static constexpr UINT k_sendResourceDescNum = 1U;
		static constexpr UINT k_defaultPageSize     = 64ULL * 1024ULL * 1024ULL;

		std::vector<CommonStruct::PendingUpload> m_pendingUploadList;
		std::vector<CommonStruct::UploadPage>    m_uploadPageList;
	};
}