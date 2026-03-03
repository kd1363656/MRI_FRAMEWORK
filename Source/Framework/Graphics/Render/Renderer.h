#pragma once

namespace FWK::Graphics
{
	class SwapChain;
}

namespace FWK::Graphics
{
	class Renderer
	{
	public:

		explicit Renderer(const Hardware& a_hardware);
		~Renderer        ();

		void Init           ();
		bool Create         ();
		void PostCreateSetup();

		void BeginFrame(const SwapChain& a_swapChain);
		void EndFrame  (const SwapChain& a_swapChain);

		const auto& GetDeirectCommandQueue() const { return m_directCommandQueue; }

	private:

		void ResetCommandObjects();

		static constexpr std::size_t k_frameCount = 3;

		const Hardware& k_hardware;

		std::vector<FrameResource> m_frameResourceList;

		DirectCommandQueue m_directCommandQueue;
		DirectCommandList  m_directCommandList;

		UINT m_frameIndex = 0U;
	};
}