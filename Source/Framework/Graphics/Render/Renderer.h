#pragma once

namespace FWK::Graphics
{
	class Renderer final
	{
	private:

		using RootSignatureMap = std::unordered_map<TypeAlias::TypeTag, std::shared_ptr<RootSignature>>;
		using PipelineStateMap = std::unordered_map<TypeAlias::TypeTag, std::shared_ptr<PipelineState>>;

	public:

		 Renderer() = default;
		~Renderer() = default;

		void INIT		();
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

		void BeginFrame();

		void BeginDraw();
		
		void Draw    (const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);
		void EndDraw (const SwapChain&						   a_swapChain);
		void EndFrame();

		void ExecuteRenderGraph(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, 
								const RTVDescriptorHeap&			     a_rtvDescriptorHeap,
								const DSVDescriptorHeap&				 a_dsvDescriptorHeap,
								const SwapChain&						 a_swapChain);

		nlohmann::json Serialize() const;

		void SetupCurrentFrameResource(const std::size_t& a_index);

		void AddFrameResource  (const std::shared_ptr<FrameResource>&   a_frameResource);
		void AddRootSignature  (const std::shared_ptr<RootSignature>&   a_rootSignature, const TypeAlias::TypeTag      a_tag);
		void AddPipelineState  (const std::shared_ptr<PipelineState>&   a_pipelineState, const TypeAlias::TypeTag      a_tag);

		std::weak_ptr<RootSignature> FindVALRootSignature(const TypeAlias::TypeTag a_tag) const;
		std::weak_ptr<PipelineState> FindVALPipelineState(const TypeAlias::TypeTag a_tag) const;

		const auto& GetREFFrameResourceList() const { return m_frameResourceList; }

		const auto& GetREFRootSignatureMap() const { return m_rootSignatureMap; }
		const auto& GetREFPipelineStateMap() const { return m_pipelineStateMap; }

		const auto& GetREFCurrentFrameResource() const { return m_currentFrameResource; }

		const auto& GetREFDirectCommandQueue() const { return m_directCommandQueue; }
		const auto& GetREFDirectCommandList () const { return m_directCommandList; }

		const auto& GetREFLightSystem() const { return m_lightSystem; }
		const auto& GetREFRenderGraph() const { return m_renderGraph; }

		const auto& GetREFRenderArea() const { return m_renderArea; }

		auto& GetMutableREFDirectCommandList() { return m_directCommandList; }
		auto& GetMutableREFRenderGraph		() { return m_renderGraph; }

	private:

		static constexpr std::size_t k_initialFrameResourceIndex   = 0ULL;
		static constexpr std::size_t k_frameResourceIndexIncrement = 1ULL;

		std::vector<std::shared_ptr<FrameResource>>	m_frameResourceList = {};

		RootSignatureMap m_rootSignatureMap = {};
		PipelineStateMap m_pipelineStateMap = {};

		std::weak_ptr<FrameResource> m_currentFrameResource = {};

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		RenderGraph m_renderGraph = {};

		LightSystem m_lightSystem = {};

		RenderArea m_renderArea = {};

		Converter::RendererJsonConverter m_rendererJsonConverter = {};

		std::size_t m_currentFrameResourceIndex = k_initialFrameResourceIndex;
	};
}