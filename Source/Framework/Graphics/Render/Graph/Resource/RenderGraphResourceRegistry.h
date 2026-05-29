#pragma once

namespace FWK::Graphics
{
	class RenderGraphResourceRegistry final
	{
	private:

		using RenderTargetTextureResourceRecordList = std::vector<std::shared_ptr<Struct::RenderGraphRenderTargetTextureResourceRecord>>;
		using RenderTargetTextureResourceRecordMap  = std::unordered_map<TypeAlias::TypeTag, std::weak_ptr<Struct::RenderGraphRenderTargetTextureResourceRecord>>;

		using DepthStencilTextureResourceRecordList = std::vector<std::shared_ptr<Struct::RenderGraphDepthStencilTextureResourceRecord>>;
		using DepthStencilTextureResourceRecordMap  = std::unordered_map<TypeAlias::TypeTag, std::weak_ptr<Struct::RenderGraphDepthStencilTextureResourceRecord>>;

	public:

		 RenderGraphResourceRegistry() = default;
		~RenderGraphResourceRegistry() = default;

		void INIT			     ();
		void Deserialize	     (const nlohmann::json& a_rootJson);
		void PostDeserializeSetup(const Struct::WindowCONFIG& a_windowCONFIG);

		bool Create(const Device&			  a_device,
					const GPUMemoryAllocator& a_gpuMemoryAllocator,
					const UINT				  a_width,
					const UINT				  a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorHeap,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

		nlohmann::json Serialize() const;

		bool Resize(const Device&							 a_device,
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const Struct::ClientSize&				 a_clientSize,
				    const UINT64&							 a_retiredFenceValue,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool,
						  DeferredResourceReleaseQueue&		 a_deferredResourceReleaseQueue);

		void AddRenderTargetTexture(const std::shared_ptr<Struct::RenderGraphRenderTargetTextureResourceRecord>& a_renderTargetTextureResourceRecord);
		void AddDepthStencilTexture(const std::shared_ptr<Struct::RenderGraphDepthStencilTextureResourceRecord>& a_depthStencilTextureResourceRecord);

		std::weak_ptr<Struct::RenderGraphRenderTargetTextureResourceRecord> FindVALRenderTargetTexture(const TypeAlias::TypeTag a_textureTag) const;
		std::weak_ptr<Struct::RenderGraphDepthStencilTextureResourceRecord> FindVALDepthStencilTexture(const TypeAlias::TypeTag a_textureTag) const;

		const auto& GetREFRenderTargetTextureResourceRecordList() const { return m_renderTargetTextureResourceRecordList; }
		const auto& GetREFDepthStencilTextureResourceRecordList() const { return m_depthStencilTextureResourceRecordList; }

	private:

		bool CreateRenderTargetTexture(const Device&											   a_device,
									   const GPUMemoryAllocator&								   a_gpuMemoryAllocator,
									   const UINT												   a_width,
									   const UINT												   a_height,
											 Struct::RenderGraphRenderTargetTextureResourceRecord& a_renderTargetTextureResourceRecord,
											 DescriptorPool<RTVDescriptorHeap>&					   a_rtvDescriptorPool,
											 DescriptorPool<SRVDescriptorHeap>&					   a_srvDescriptorPool);

		bool CreateDepthStencilTexture(const Device&											   a_device,
									   const GPUMemoryAllocator&								   a_gpuMemoryAllocator,
									   const UINT												   a_width,
									   const UINT												   a_height,
											 Struct::RenderGraphDepthStencilTextureResourceRecord& a_depthStencilTextureResourceRecord,
											 DescriptorPool<DSVDescriptorHeap>&					   a_dsvDescriptorPool);

		RenderTargetTextureResourceRecordList m_renderTargetTextureResourceRecordList = {};
		RenderTargetTextureResourceRecordMap  m_renderTargetTextureResourceRecordMap  = {};

		DepthStencilTextureResourceRecordList m_depthStencilTextureResourceRecordList = {};
		DepthStencilTextureResourceRecordMap  m_depthStencilTextureResourceRecordMap  = {};

		Converter::RenderGraphResourceRegistryJsonConverter m_renderGraphResourceRegistryJsonConverter = {};
	};
}