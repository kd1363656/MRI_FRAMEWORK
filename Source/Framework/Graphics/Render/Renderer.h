#pragma once

namespace FWK::Graphics
{
	class IDrawCommand;
}

namespace FWK::Graphics
{
	class Renderer final
	{
	private:

		using RootSignatureMap = std::unordered_map<TypeAlias::TypeTag, RootSignature>;
		using PipelineStateMap = std::unordered_map<TypeAlias::TypeTag, PipelineState>;
		using DrawCommandMap   = std::unordered_map<TypeAlias::TypeTag, std::weak_ptr<IDrawCommand>>;

	public:

		 Renderer() = default;
		~Renderer() = default;

		void Deserialize    (const nlohmann::json& a_rootJson);
		bool Create         (const Device&		   a_device, const ShaderCompiler& a_shaderCompiler);
		void PostCreateSetup(const SwapChain&	   a_swapChain);

		void BeginFrame() const;

		void BeginDraw(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap);
		
		void Draw    (const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, TextureSystem& a_textureSystem) const;
		void EndDraw (const SwapChain&						   a_swapChain);
		void EndFrame();

		nlohmann::json Serialize() const;

		void AddRootSignature  (const RootSignature&                 a_rootSignature, const TypeAlias::TypeTag      a_tag);
		void AddPipelineState  (const PipelineState&                 a_pipelineState, const TypeAlias::TypeTag      a_tag);
		void AddDrawCommandMap (const std::shared_ptr<IDrawCommand>& a_drawCommand,   const TypeAlias::StaticTypeID a_staticTypeID);
		void AddDrawCommandList(const std::shared_ptr<IDrawCommand>& a_drawCommand);

		const RootSignature* FindPTRRootSignature(const TypeAlias::TypeTag a_tag) const;
		const PipelineState* FindPTRPipelineState(const TypeAlias::TypeTag a_tag) const;

		template <typename Type>
		std::shared_ptr<Type> FetchVALDrawCommand() const 
		{
			if (const auto& l_itr = m_drawCommandMap.find(Type::GetTypeINFO().k_staticTypeID);
				l_itr != m_drawCommandMap.end())
			{
				if (auto l_drawCommand = l_itr->second.lock())
				{
					return std::static_pointer_cast<Type>(l_drawCommand);
				}
			}

			return std::shared_ptr<Type>();
		}

		const auto& GetREFDirectCommandQueue() const { return m_directCommandQueue; }
		const auto& GetREFDirectCommandList () const { return m_directCommandList; }

		const auto& GetREFRootSignatureMap() const { return m_rootSignatureMap; }
		const auto& GetREFPipelineStateMap() const { return m_pipelineStateMap; }
		const auto& GetREFDrawCommandMap  () const { return m_drawCommandMap; }
		const auto& GetREFDrawCommandList () const { return m_drawCommandList; }

		const auto& GetREFResourceList() const { return m_frameResourceList; }

		auto& GetMutableREFFrameResourceList() { return m_frameResourceList; }

	private:

		const FrameResource* FetchPTRCurrentFrameResource() const;

		FrameResource* FetchMutablePTRCurrentFrameResource();

		static constexpr std::size_t k_initialFrameResourceIndex   = 0ULL;
		static constexpr std::size_t k_frameResourceIndexIncrement = 1ULL;

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		RenderArea m_renderArea = {};

		JsonConverter::RendererJsonConverter m_rendererJsonConverter = {};

		RootSignatureMap m_rootSignatureMap = {};
		PipelineStateMap m_pipelineStateMap = {};
		DrawCommandMap   m_drawCommandMap   = {};

		std::vector<FrameResource>				   m_frameResourceList = {};
		std::vector<std::shared_ptr<IDrawCommand>> m_drawCommandList   = {};

		std::size_t m_currentFrameResourceIndex = k_initialFrameResourceIndex;
	};
}