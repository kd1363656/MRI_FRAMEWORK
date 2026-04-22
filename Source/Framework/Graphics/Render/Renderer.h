#pragma once

namespace FWK::Graphics
{
	class Renderer final
	{
	public:

		 Renderer() = default;
		~Renderer() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     (const Device&		   a_device);

		void BeginDraw(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap);
		
		void EndDraw (const SwapChain& a_swapChain);
		void EndFrame();

		nlohmann::json Serialize() const;

		auto& GetMutableREFFrameResourceList() { return m_frameResourceList; }
		
		const auto& GetREFDirectCommandQueue() const { return m_directCommandQueue; }

		const auto& GetREFResourceList() const { return m_frameResourceList; }

	private:

		const FrameResource* FetchPTRCurrentFrameResource() const;

		FrameResource* FetchMutablePTRCurrentFrameResource();

		static constexpr UINT k_incrementCurrentFrameIndex = 1U;

		UINT m_currentFrameResourceIndex = 0U;

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		JsonConverter::RendererJsonConverter m_rendererJsonConverter = {};

		std::vector<FrameResource> m_frameResourceList = {};
	};
}