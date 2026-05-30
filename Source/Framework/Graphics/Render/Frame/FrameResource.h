#pragma once

namespace FWK::Graphics
{
	class FrameResource final
	{
	private:

		using ConstantBufferUploaderMap = std::unordered_map<TypeAlias::StaticTypeID, std::shared_ptr<ConstantBufferUploaderBase>>;

	public:

		 FrameResource() = default;
		~FrameResource() = default;

		void INIT                ();
		void Deserialize         (const nlohmann::json&		  a_rootJson);
		void PostDeserializeSetup(const Struct::WindowCONFIG& a_windowCONFIG);

		bool Create(const Device&							 a_device,
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const UINT								 a_width,
					const UINT								 a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

		void BeginFrame() const;

		nlohmann::json Serialize() const;

		bool Resize(const Device&							 a_device,
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const Struct::ClientSize&				 a_clientSize,
					const UINT64&							 a_retiredFenceValue,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool,
						  DeferredResourceReleaseQueue&		 a_deferredResourceReleaseQueue);

		void AddConstantBufferUploader(const std::shared_ptr<ConstantBufferUploaderBase>& a_constantBufferUploader);

		template <typename Type>
		std::weak_ptr<ConstantBufferUploaderBase> FindPTRConstantBufferUploader() const
		{
			const auto& l_itr = m_constantBufferUploaderMap.find(Type::GetREFTypeINFO().k_staticTypeID);

			if (l_itr == m_constantBufferUploaderMap.end()) { return std::weak_ptr<ConstantBufferUploaderBase>(); }

			if (!l_itr->second) { return std::weak_ptr<ConstantBufferUploaderBase>(); }

			return l_itr->second;
		}
		
		const auto& GetREFConstantBufferUploaderMap() const { return m_constantBufferUploaderMap; }

		const auto& GetREFDirectCommandAllocator() const { return m_directCommandAllocator; }

		const auto& GetREFRenderGraphResourceRegistry() const { return m_renderGraphFrameResourceRegistry; }

		auto& GetMutableREFDirectCommandAllocator     () { return m_directCommandAllocator; }

		auto& GetMutableREFRenderGraphFrameResourceRegistry() { return m_renderGraphFrameResourceRegistry; }

	private:

		std::shared_ptr<DirectCommandAllocator> m_directCommandAllocator = nullptr;

		ConstantBufferUploaderMap m_constantBufferUploaderMap = {};

		RenderGraphFrameResourceRegistry m_renderGraphFrameResourceRegistry = {};

		Converter::FrameResourceJsonConverter m_frameResourceJsonConverter = {};
	};
}