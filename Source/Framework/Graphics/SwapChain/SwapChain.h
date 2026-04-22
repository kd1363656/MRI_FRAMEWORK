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
					const Device&							 a_device,
					const Factory&                           a_factory,
					const DirectCommandQueue&                a_directCommandQueue,
					const Struct::WindowCONFIG&              a_windowCONFIG,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		void PostCreateSetup(const HWND& a_hWND, const Factory& a_factory) const;

		void Present() const;

		nlohmann::json Serialize() const;

		void ResizeBackBufferList(const std::size_t a_backBufferNum);

		void SetSyncInterval(const UINT a_set) { m_syncInterval = a_set; }

		UINT FetchVALCurrentBackBufferIndex() const;

		const auto& GetREFSwapChain() const { return m_swapChain; }

		const auto& GetREFBackBufferList() const { return m_backBufferList; }

		auto GetVALSyncInterval() const { return m_syncInterval; }

	private:

		bool CreateSwapChain(const HWND&                 a_hwnd,
							 const Factory&              a_factory,
							 const DirectCommandQueue&   a_directCommandQueue,
							 const Struct::WindowCONFIG& a_windowConfig);

		bool CreateBackBufferList(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		static constexpr DXGI_FORMAT k_swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		static constexpr UINT k_defaultSampleCount       = 1U;
		static constexpr UINT k_defaultSampleQuality     = 0U;
		static constexpr UINT k_swapChainPresentFlagNone = 0U;
		static constexpr UINT k_swapChainDescFlags	     = 0U;

		TypeAlias::ComPtr<IDXGISwapChain4> m_swapChain = nullptr;

		UINT m_syncInterval = Constant::k_defaultSyncInterval;

		JsonConverter::SwapChainJsonConverter m_swapChainJsonConverter = {};

		std::vector<Struct::BackBuffer> m_backBufferList = {};
	};
}