#pragma once

namespace FWK::Graphics
{
	class ResourceContext final
	{
	public:

		ResourceContext() = default;
		~ResourceContext() = default;

		void Init();
		bool Create(const Device& a_device);

		void Deserialize(const nlohmann::json& a_json);
		nlohmann::json Serialize();

		TextureID RegisterTexture(const Device& a_device, const std::string& a_sourcePath);
		bool UnregisterTexture(const TextureID a_textureID);

		bool IsRegisteredTexture(const TextureID a_textureID) const;

		TextureID FindTextureID(const std::string_view& a_sourcePath)					  const;
		UINT FindTextureSRVIndex(const TextureID a_textureID)							  const;
		const CommonStruct::TextureRecord* FindTextureRecord(const TextureID a_textureID) const;
		CommonStruct::TextureRecord* FindMutableTextureRecord(const TextureID a_textureID);

		const auto& GetDescriptorHeapContext() const { return m_descriptorHeapContext; }
		const auto& GetTextureContext()        const { return m_textureContext; }

		auto& GetMutableDescriptorHeapContext() { return m_descriptorHeapContext; }

	private:

		static constexpr UINT64 k_defaultTextureHeapPageSize = 64ULL * 1024ULL * 1024ULL;

		DescriptorHeapContext m_descriptorHeapContext = DescriptorHeapContext();

		TextureContext m_textureContext = TextureContext();

		TextureHeapAllocator m_textureHeapAllocator = TextureHeapAllocator();
		TextureUploader      m_textureUploader = TextureUploader();
	};
}
