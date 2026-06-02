#pragma once

namespace FWK::Graphics
{
	class RenderGraph final
	{
	private:

		using DrawRequestPassMap      = std::unordered_map<TypeAlias::StaticTypeID, std::weak_ptr<DrawRequestPassBase>>;
		using DrawRequestPerObjectMap = std::unordered_map<TypeAlias::StaticTypeID, std::weak_ptr<DrawRequestPerObjectBase>>;

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

		void INIT		();
		void Deserialize(const nlohmann::json& a_rootJson);

		void PostCreateSetup(Renderer& a_renderer) const;

		bool Compile();

		void BeginFrame() const;

		void Execute(const RTVDescriptorHeap&				  a_rtvDescriptorHeap,
					 const DSVDescriptorHeap&				  a_dsvDescriptorHeap,
					 const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
					 const SwapChain&						  a_swapChain,
					 const TextureSystem&					  a_textureSystem,
					 	   DirectCommandList&				  a_directCommandList,
					 	   Renderer&						  a_renderer);

		nlohmann::json Serialize() const;

		void AddPass		        (	   std::unique_ptr<IRenderGraphPass>&&		  a_pass);
		void AddDrawRequestPass     (const std::shared_ptr<DrawRequestPassBase>&      a_drawRequestPass);
		void AddDrawRequestPerObject(const std::shared_ptr<DrawRequestPerObjectBase>& a_drawRequestPerObject);

		template <Concept::IsDerivedDrawRequestPassBaseConcept DrawRequestPassType>
		std::weak_ptr<DrawRequestPassType> FindVALDrawRequestPass() const 
		{
			const auto l_staticTypeID = DrawRequestPassType::GetREFTypeINFO().k_staticTypeID;

			const auto& l_itr = m_drawRequestPassMap.find(l_staticTypeID);

			if (l_itr == m_drawRequestPassMap.end()) { return {}; }

			const auto l_drawRequestPass = l_itr->second.lock();

			if (!l_drawRequestPass) { return {}; }

			return std::static_pointer_cast<DrawRequestPassType>(l_drawRequestPass);
		}
		template <Concept::IsDerivedDrawRequestPerObjectBaseConcept DrawRequestPerObjectType>
		std::weak_ptr<DrawRequestPerObjectType> FindVALDrawRequestPerObject() const 
		{
			const auto l_staticTypeID = DrawRequestPerObjectType::GetREFTypeINFO().k_staticTypeID;

			const auto& l_itr = m_drawRequestPerObjectMap.find(l_staticTypeID);

			if (l_itr == m_drawRequestPerObjectMap.end()) { return {}; }

			const auto l_drawRequestPerObject = l_itr->second.lock();

			if (!l_drawRequestPerObject) { return {}; }

			return std::static_pointer_cast<DrawRequestPerObjectType>(l_drawRequestPerObject);
		}

		const auto& GetREFPassList			      () const { return m_passList; }
		const auto& GetREFDrawRequestPassList     () const { return m_drawRequestPassList; }
		const auto& GetREFDrawRequestPerObjectList() const { return m_drawRequestPerObjectList; }

	private:

		bool IsReadAccess (const Struct::RenderGraphTextureAccess& a_textureAccess) const;
		bool IsWriteAccess(const Struct::RenderGraphTextureAccess& a_textureAccess) const;

		void AddDependencyEdge(const std::uint32_t						      a_fromPassIndex, 
							   const std::uint32_t						      a_toPassIndex,
									 std::vector<std::vector<std::uint32_t>>& a_edgeList,
									 std::vector<std::uint32_t>&			  a_inDegreeList) const;

		void BuildDependency(std::vector<std::vector<std::uint32_t>>& a_edgeList, std::vector<std::uint32_t>& a_inDegreeList) const;

		void TransitionPassTexture(const IRenderGraphPass& a_pass, DirectCommandList& a_directCommandList, Renderer& a_renderer);

		bool TransitionRenderTargetTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry, const DirectCommandList& a_directCommandList);
		bool TransitionDepthStencilTexture(const Struct::RenderGraphTextureAccess& a_textureAccess, const RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry, const DirectCommandList& a_directCommandList);
		 
		D3D12_RESOURCE_STATES ConvertTextureUsageToResourceState(const TypeAlias::TypeTag a_usageTag) const;

		static constexpr std::uint32_t k_noRenderGraphIncomingEdgeCount = 0U;
		static constexpr std::uint32_t k_invalidRenderGraphPassIndex	= UINT32_MAX; 

		std::vector<std::unique_ptr<IRenderGraphPass>> m_passList = {};
		
		std::vector<std::shared_ptr<DrawRequestPassBase>> m_drawRequestPassList = {};
		DrawRequestPassMap								  m_drawRequestPassMap  = {};

		std::vector<std::shared_ptr<DrawRequestPerObjectBase>> m_drawRequestPerObjectList = {};
		DrawRequestPerObjectMap								   m_drawRequestPerObjectMap  = {};

		std::vector<std::uint32_t> m_sortedPassIndexList = {};

		Converter::RenderGraphJsonConverter m_renderGraphJsonConverter = {};
	};
}