#pragma once

namespace FWK::Graphics
{
	class DrawCommandBase;
}

namespace FWK::Graphics
{
	class Renderer final
	{
	private:

		using DrawCommandMap   = std::unordered_map<TypeAlias::TypeTag, std::weak_ptr<DrawCommandBase>>;
		using RootSignatureMap = std::unordered_map<TypeAlias::TypeTag, std::shared_ptr<RootSignature>>;
		using PipelineStateMap = std::unordered_map<TypeAlias::TypeTag, std::shared_ptr<PipelineState>>;

	public:

		 Renderer() = default;
		~Renderer() = default;

		void Init		();
		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Device&							 a_device, 
					const ShaderCompiler&					 a_shaderCompiler,
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const UINT								 a_width,
					const UINT								 a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

		void PostCreateSetup(const SwapChain& a_swapChain);

		void BeginFrame() const;

		void BeginDraw(const RTVDescriptorHeap& a_rtvDescriptorHeap, const DSVDescriptorHeap& a_dsvDescriptorHeap);
		
		void Draw    (const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);
		void EndDraw (const SwapChain&						   a_swapChain);
		void EndFrame();

		nlohmann::json Serialize() const;

		void SetupCurrentFrameResource(const std::size_t& a_index);

		void AddFrameResource  (const std::shared_ptr<FrameResource>&   a_frameResource);
		void AddDrawCommandList(const std::shared_ptr<DrawCommandBase>& a_drawCommand);
		void AddDrawCommandMap (const std::shared_ptr<DrawCommandBase>& a_drawCommand,   const TypeAlias::StaticTypeID a_staticTypeID);
		void AddRootSignature  (const std::shared_ptr<RootSignature>&   a_rootSignature, const TypeAlias::TypeTag      a_tag);
		void AddPipelineState  (const std::shared_ptr<PipelineState>&   a_pipelineState, const TypeAlias::TypeTag      a_tag);

		std::weak_ptr<RootSignature> FindVALRootSignature(const TypeAlias::TypeTag a_tag) const;
		std::weak_ptr<PipelineState> FindVALPipelineState(const TypeAlias::TypeTag a_tag) const;

		template <Concept::IsDerivedDrawCommandBaseConcept Type>
		std::shared_ptr<Type> FindVALDrawCommand() const 
		{
			if (const auto& l_itr = m_drawCommandMap.find(Type::GetTypeINFO().k_staticTypeID);
				l_itr != m_drawCommandMap.end())
			{
				if (auto l_drawCommand = l_itr->second.lock())
				{
					return std::static_pointer_cast<Type>(l_drawCommand);
				}
			}

			return nullptr;
		}

		const auto& GetREFFrameResourceList() const { return m_frameResourceList; }
		const auto& GetREFDrawCommandList  () const { return m_drawCommandList; }

		const auto& GetREFDrawCommandMap  () const { return m_drawCommandMap; }
		const auto& GetREFRootSignatureMap() const { return m_rootSignatureMap; }
		const auto& GetREFPipelineStateMap() const { return m_pipelineStateMap; }

		const auto& GetREFCurrentFrameResource() const { return m_currentFrameResource; }

		const auto& GetREFDirectCommandQueue() const { return m_directCommandQueue; }
		const auto& GetREFDirectCommandList () const { return m_directCommandList; }

		const auto& GetREFLightSystem() const { return m_lightSystem; }

		const auto& GetREFRenderArea() const { return m_renderArea; }

		auto& GetMutableREFDirectCommandList() { return m_directCommandList; }

	private:

		static constexpr std::size_t k_initialFrameResourceIndex   = 0ULL;
		static constexpr std::size_t k_frameResourceIndexIncrement = 1ULL;

		std::vector<std::shared_ptr<FrameResource>>	  m_frameResourceList = {};
		std::vector<std::shared_ptr<DrawCommandBase>> m_drawCommandList   = {};

		DrawCommandMap   m_drawCommandMap   = {};
		RootSignatureMap m_rootSignatureMap = {};
		PipelineStateMap m_pipelineStateMap = {};

		std::weak_ptr<FrameResource> m_currentFrameResource = {};

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		LightSystem m_lightSystem = {};

		RenderArea m_renderArea = {};

		Converter::RendererJsonConverter m_rendererJsonConverter = {};

		std::size_t m_currentFrameResourceIndex = k_initialFrameResourceIndex;
	};
}