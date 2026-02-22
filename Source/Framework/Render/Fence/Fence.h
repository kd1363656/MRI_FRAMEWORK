#pragma once

namespace FWK::Render
{
	class Fence
	{
	public:

		explicit Fence(const Hardware& a_hardware, const GraphicsCommandQueue& a_graphicsCommandQueue);
		~Fence        ();

		void Init  ();
		bool Create();

	private:

		void WaitForGPUIdle();

		const Hardware&				k_hardware;
		const GraphicsCommandQueue& k_graphicsCommandQueue;

		TypeDEF::ComPtr<ID3D12Fence1> m_fence;

		HANDLE m_fenceEvent;

		UINT64 m_fenceValue;
	};
}