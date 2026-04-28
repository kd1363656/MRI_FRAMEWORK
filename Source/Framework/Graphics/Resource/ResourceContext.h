#pragma once

namespace FWK::Graphics
{
	class ResourceContext final
	{
	public:

		 ResourceContext() = default;
		~ResourceContext() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    (const Device&		   a_device);

		nlohmann::json Serialize() const;

		const auto& GetREFRTVDescriptorPool() const { return m_rtvDescriptorPool; }
		const auto& GetREFSRVDescriptorPool() const { return m_srvDescriptorPool; }

		const auto& GetREFGPUMemoryAllocator() const { return m_gpuMemoryAllocator; }
		const auto& GetREFTextureSystem     () const { return m_textureSystem; }

		auto& GetMutableREFRTVDescriptorPool() { return m_rtvDescriptorPool; }
		auto& GetMutableREFSRVDescriptorPool() { return m_srvDescriptorPool; }

		auto& GetMutableREFGPUMemoryAllocator() { return m_gpuMemoryAllocator; }
		auto& GetMutableREFTextureSystem     () { return m_textureSystem; }

	private:

		DescriptorPool<RTVDescriptorHeap> m_rtvDescriptorPool = {};
		DescriptorPool<SRVDescriptorHeap> m_srvDescriptorPool = {};
		
		GPUMemoryAllocator m_gpuMemoryAllocator = {};
		UploadSystem       m_uploadSystem       = {};
		TextureSystem      m_textureSystem      = {};

		JsonConverter::ResourceContextJsonConverter m_resourceContextJsonConverter = {};
	};
}