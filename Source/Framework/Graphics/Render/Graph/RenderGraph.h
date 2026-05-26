#pragma once

namespace FWK::Graphics
{
	class RenderGraph final
	{
	private:

		struct TextureAccessPassRecord final
		{
			TypeAlias::TypeTag m_textureTag = Constant::k_invalidTypeTag;

			std::uint32_t m_passIndex	= 0U;
			std::uint32_t m_accessOrder = 0U;

			bool m_isRead  = false;
			bool m_isWrite = false;
		};

	public:

		 RenderGraph() = default;
		~RenderGraph() = default;

		void INIT();

		void PostCreateSetup(Renderer& a_renderer);

		bool Compile();

		void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
					 const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
					 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
					 const SwapChain&						  a_swapChain,
					 	   DirectCommandList&				  a_directCommandList,
					 	   Renderer&						  a_renderer);

		template <typename Type, typename... Args>
		Type& AddPass(Args&&... a_args)
		{
			auto l_pass = std::make_unique<Type>(std::forward<Args>(a_args)...);

			Type& l_passREF = *l_pass;

			m_passList.emplace_back(std::move(l_pass));

			return l_passREF;
		}

		void AddPass(std::unique_ptr<IRenderGraphPass>&& a_pass);

		const auto& GetREFPassList() const { return m_passList; }

	private:

		bool IsReadAccess (const Struct::RenderGraphTextureAccess& a_textureAccess) const;
		bool IsWriteAccess(const Struct::RenderGraphTextureAccess& a_textureAccess) const;

		void AddDependencyEdge(const std::uint32_t						      a_fromPassIndex, 
							   const std::uint32_t						      a_toPassIndex,
									 std::vector<std::vector<std::uint32_t>>& a_edgeList,
									 std::vector<std::uint32_t>&			  a_inDegreeList) const;

		void BuildDependency(std::vector<std::vector<std::uint32_t>>& a_edgeList, std::vector<std::uint32_t>& a_inDegreeList) const;

		void TransitionPassTexture(const IRenderGraphPass& a_pass, DirectCommandList& a_directCommandList, Renderer& a_renderer);

		bool TransitionRenderTargetTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphResourceRegistry& a_renderGraphResourceRegistry, const DirectCommandList& a_directCommandList);
		bool TransitionDepthStencilTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphResourceRegistry& a_renderGraphResourceRegistry, const DirectCommandList& a_directCommandList);
		 
		D3D12_RESOURCE_STATES ConvertTextureUsageToResourceState(const TypeAlias::TypeTag a_usageTag) const;

		static constexpr std::uint32_t k_noRenderGraphIncomingEdgeCount = 0U;
		static constexpr std::uint32_t k_invalidRenderGraphPassIndex	= UINT32_MAX; 

		std::vector<std::unique_ptr<IRenderGraphPass>> m_passList = {};

		std::vector<std::uint32_t> m_sortedPassIndexList = {};
	};
}