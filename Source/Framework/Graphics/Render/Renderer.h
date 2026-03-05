#pragma once

namespace FWK::Graphics
{
	class SwapChain;
	class RTVDescriptorHeap;
}

namespace FWK::Graphics
{
	class Renderer
	{
	public:

		explicit Renderer(const Hardware& a_hardware, const ShaderCompiler& a_shaderCompiler);
		~Renderer        ();

		void Init           ();
		bool Create         ();
		void PostCreateSetup(const SwapChain& a_swapChain);

		void BeginFrame(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap);
		void Draw      ();
		void EndFrame  (const SwapChain& a_swapChain);

		const auto& GetDeirectCommandQueue() const { return m_directCommandQueue; }

	private:

		void ResetCommandObjects(const DirectCommandAllocator& a_directCommandAllocator);

		static constexpr std::size_t k_frameCount = 3;

		const Hardware& k_hardware;

		std::vector<FrameResource> m_frameResourceList;

		DirectCommandQueue m_directCommandQueue;
		DirectCommandList  m_directCommandList;

		RenderArea    m_renderArea;
		RootSignature m_rootSignature;
		PipelineState m_pipelineState;

		UINT m_frameIndex = 0U;
	};
}