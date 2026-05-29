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

		void PostCreateSetup(const HWND& a_hwnd, const Factory& a_factory) const;

		void Present() const;

		nlohmann::json Serialize() const;

		// ※注意
		// 呼び出す前にGPUとの完全同期を取ること
		bool Resize(const Device& a_device, const Struct::ClientSize& a_clientSize, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		void ResizeBackBufferList(const std::size_t a_backBufferNum);

		void SetSyncInterval(const UINT a_set) { m_syncInterval = a_set; }

		UINT FetchVALCurrentBackBufferIndex() const;

		const auto& GetREFBackBufferList() const { return m_backBufferList; }

		const auto& GetREFSwapChain() const { return m_swapChain; }

		auto GetVALSyncInterval() const { return m_syncInterval; }

	private:

		bool CreateSwapChain(const HWND&                 a_hwnd,
							 const Factory&              a_factory,
							 const DirectCommandQueue&   a_directCommandQueue,
							 const Struct::WindowCONFIG& a_windowConfig);

		bool CreateBackBufferList(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		bool IsValidBackBufferSize(const Struct::ClientSize& a_clientSize) const;

		void ReleaseBackBufferList(DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		static constexpr std::uint32_t k_invalidBackBufferWidth  = 0U;
		static constexpr std::uint32_t k_invalidBackBufferHeight = 0U;

		static constexpr UINT k_swapChainPresentFlagNone = 0U;
		static constexpr UINT k_swapChainDescFlags	     = 0U;

		std::vector<Struct::BackBuffer> m_backBufferList = {};

		TypeAlias::ComPtr<IDXGISwapChain4> m_swapChain = nullptr;

		Converter::SwapChainJsonConverter m_swapChainJsonConverter = {};

		UINT m_syncInterval = Constant::k_defaultSyncInterval;
	};
}