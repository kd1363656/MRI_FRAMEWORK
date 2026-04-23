#pragma once

namespace FWK::Graphics
{
	class Renderer final
	{
	private:

		using RootSignatureMap = std::unordered_map<TypeAlias::TypeTag, RootSignature>;

	public:

		 Renderer() = default;
		~Renderer() = default;

		void Deserialize    (const nlohmann::json& a_rootJson);
		bool Create         (const Device&		   a_device);
		void PostCreateSetup(const SwapChain&	   a_swapChain);

		void BeginDraw(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap);
		
		void EndDraw (const SwapChain& a_swapChain);
		void EndFrame();

		nlohmann::json Serialize() const;

		void AddRootSignature(const RootSignature& a_rootSignature, const TypeAlias::TypeTag a_tag);
		
		const RootSignature* FindPTRRootSignature(const TypeAlias::TypeTag a_tag) const;

		auto& GetMutableREFFrameResourceList() { return m_frameResourceList; }
		
		const auto& GetREFDirectCommandQueue() const { return m_directCommandQueue; }

		const auto& GetREFRootSignatureMap() const { return m_rootSignatureMap; }

		const auto& GetREFResourceList() const { return m_frameResourceList; }

	private:

		const FrameResource* FetchPTRCurrentFrameResource() const;

		FrameResource* FetchMutablePTRCurrentFrameResource();

		static constexpr UINT k_incrementCurrentFrameIndex = 1U;

		UINT m_currentFrameResourceIndex = 0U;

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		RenderArea m_renderArea = {};

		JsonConverter::RendererJsonConverter m_rendererJsonConverter = {};

		RootSignatureMap m_rootSignatureMap = {};

		std::vector<FrameResource> m_frameResourceList = {};
	};
}