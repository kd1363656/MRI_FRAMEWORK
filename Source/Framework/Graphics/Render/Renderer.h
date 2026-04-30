#pragma once

namespace FWK::Graphics
{
	class Renderer final
	{
	private:

		using RootSignatureMap = std::unordered_map<TypeAlias::TypeTag, RootSignature>;
		using PipelineStateMap = std::unordered_map<TypeAlias::TypeTag, PipelineState>;

	public:

		 Renderer() = default;
		~Renderer() = default;

		void Deserialize    (const nlohmann::json& a_rootJson);
		bool Create         (const Device&		   a_device, const ShaderCompiler& a_shaderCompiler);
		void PostCreateSetup(const SwapChain&	   a_swapChain);

		void BeginDraw(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap);
		
		void SetupGraphicsPipelineByTag(const TypeAlias::TypeTag a_pipelineStateTag) const;

		void Draw    () const;
		void EndDraw (const SwapChain& a_swapChain);
		void EndFrame();

		nlohmann::json Serialize() const;

		void AddRootSignature(const RootSignature& a_rootSignature, const TypeAlias::TypeTag a_tag);
		void AddPipelineState(const PipelineState& a_pipelineState, const TypeAlias::TypeTag a_tag);

		const RootSignature* FindPTRRootSignature(const TypeAlias::TypeTag a_tag) const;
		const PipelineState* FindPTRPipelineState(const TypeAlias::TypeTag a_tag) const;

		const auto& GetREFDirectCommandQueue() const { return m_directCommandQueue; }

		const auto& GetREFRootSignatureMap() const { return m_rootSignatureMap; }
		const auto& GetREFPipelineStateMap() const { return m_pipelineStateMap; }

		const auto& GetREFResourceList() const { return m_frameResourceList; }

		auto& GetMutableREFFrameResourceList() { return m_frameResourceList; }

	private:

		const FrameResource* FetchPTRCurrentFrameResource() const;

		FrameResource* FetchMutablePTRCurrentFrameResource();

		static constexpr D3D12_RESOURCE_STATES k_backBufferPresentState      = D3D12_RESOURCE_STATE_PRESENT;
		static constexpr D3D12_RESOURCE_STATES k_backBufferRenderTargetState = D3D12_RESOURCE_STATE_RENDER_TARGET;

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		static constexpr std::size_t k_initialFrameResourceIndex   = 0ULL;
		static constexpr std::size_t k_frameResourceIndexIncrement = 1ULL;

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		RenderArea m_renderArea = {};

		JsonConverter::RendererJsonConverter m_rendererJsonConverter = {};

		RootSignatureMap m_rootSignatureMap = {};
		PipelineStateMap m_pipelineStateMap = {};

		std::vector<FrameResource> m_frameResourceList = {};

		std::size_t m_currentFrameResourceIndex = k_initialFrameResourceIndex;
	};
}