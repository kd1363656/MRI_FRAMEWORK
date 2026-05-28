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

		void BeginFrame(const DirectCommandQueue& a_directCommandQueue);

		nlohmann::json Serialize() const;

		const auto& GetREFRTVDescriptorPool() const { return m_rtvDescriptorPool; }
		const auto& GetREFSRVDescriptorPool() const { return m_srvDescriptorPool; }
		const auto& GetREFDSVDescriptorPool() const { return m_dsvDescriptorPool; }

		const auto& GetREFDeferredResourceReleaseQueue() const { return m_deferredResourceReleaseQueue; }

		const auto& GetREFGPUMemoryAllocator() const { return m_gpuMemoryAllocator; }
		const auto& GetREFUploadSystem      () const { return m_uploadSystem; }
		const auto& GetREFTextureSystem     () const { return m_textureSystem; }
		const auto& GetREFStaticModelSystem () const { return m_staticModelSystem; }

		auto& GetMutableREFRTVDescriptorPool() { return m_rtvDescriptorPool; }
		auto& GetMutableREFSRVDescriptorPool() { return m_srvDescriptorPool; }
		auto& GetMutableREFDSVDescriptorPool() { return m_dsvDescriptorPool; }

		auto& GetMutableREFDeferredResourceReleaseQueue() { return m_deferredResourceReleaseQueue; }

		auto& GetMutableREFGPUMemoryAllocator() { return m_gpuMemoryAllocator; }
		auto& GetMutableREFUploadSystem	     () { return m_uploadSystem; }
		auto& GetMutableREFTextureSystem     () { return m_textureSystem; }
		auto& GetMutableREFStaticModelSystem () { return m_staticModelSystem; }

	private:

		DescriptorPool<RTVDescriptorHeap> m_rtvDescriptorPool = {};
		DescriptorPool<SRVDescriptorHeap> m_srvDescriptorPool = {};
		DescriptorPool<DSVDescriptorHeap> m_dsvDescriptorPool = {};

		DeferredResourceReleaseQueue m_deferredResourceReleaseQueue = {};

		GPUMemoryAllocator m_gpuMemoryAllocator = {};
		UploadSystem       m_uploadSystem       = {};
		TextureSystem      m_textureSystem      = {};
		StaticModelSystem  m_staticModelSystem  = {};
		
		Converter::ResourceContextJsonConverter m_resourceContextJsonConverter = {};
	};
}