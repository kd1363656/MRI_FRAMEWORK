#pragma once

namespace FWK::Graphics
{
	class SwapChain final
	{
	public:

		 SwapChain() = default;
		~SwapChain() = default;

		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const HWND&                              a_hwnd,
					const Device&                            a_device,
					const DirectCommandQueue&                a_directCommandQueue,
					const Struct::WindowCONFIG&              a_windowCONFIG,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		void PostCreateSetup(const HWND& a_hWND, const Factory& a_factory);

		nlohmann::json Serialize() const;

	private:

		bool CreateSwapChain(const HWND&                 a_hwnd,
							 const Factory&              a_factory,
							 const DirectCommandQueue&   a_directCommandQueue,
							 const Struct::WindowCONFIG& a_windowCONFIG);

		bool CreateBackBufferList(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		static constexpr DXGI_FORMAT k_swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		UINT m_backBufferNUM = Constant::k_defaultBackBufferNUM;
		
		TypeAlias::ComPtr<IDXGISwapChain4> m_swapChain = nullptr;
	};
}