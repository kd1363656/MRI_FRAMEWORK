#pragma once

namespace FWK::Render
{
	class FrameResource;
	class SwapChain;
	class Fence;
}

namespace FWK::Render
{
	class GraphicsCommandContext
	{
	public:

		explicit GraphicsCommandContext(const Hardware& a_hardware);
		~GraphicsCommandContext        ();

		void Init  ();
		bool Create();

		void ResetCommandObjects(const std::vector<FrameResource>& a_frameResourceList, const SwapChain& a_swapChain) const;

		void EndFrame(const Fence& a_fence, const UINT a_executeCommandListNum) const;

		const auto& GetHardware() const { return k_hardware; }

		const auto& GetGraphicsCommandQueue() const { return m_graphicsCommandQueue; }
		const auto& GetGraphicsCommandList () const { return m_graphicsCommandList; }

	private:

		const Hardware& k_hardware;

		GraphicsCommandQueue m_graphicsCommandQueue;
		GraphicsCommandList  m_graphicsCommandList;
	};
}