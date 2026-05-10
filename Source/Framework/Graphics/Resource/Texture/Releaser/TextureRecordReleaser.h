#pragma once

namespace FWK::Graphics
{
	class TextureRecordReleaser final
	{
	public:

		explicit TextureRecordReleaser(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) : 
			m_srvDescriptorPool(a_srvDescriptorPool)
		{}
		~TextureRecordReleaser() = default;

		bool ReleaseRecord(Struct::TextureRecord& a_textureRecord) const;

	private:

		DescriptorPool<SRVDescriptorHeap>& m_srvDescriptorPool;
	};
}