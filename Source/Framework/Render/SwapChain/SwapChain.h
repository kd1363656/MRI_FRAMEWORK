#pragma once

namespace FWK::Render
{
	class SwapChain final
	{
	public:

		explicit SwapChain(const Hardware& a_hardware, const GraphicsCommandQueue& a_graphicsCommandQueue);
		~SwapChain        ();

		void Init  ();
		bool Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig);

		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize  ();

		void Present(const UINT a_syncInterval) const;

		const auto& GetBackBufferList() const { return m_backBufferList; }

		const auto& GetRTVHeap() const { return m_rtvHeap; }

		UINT GetCurrentBackBufferIndex() const;
		UINT GetBackBufferNum         () const { return m_backBufferNum; }

		void SetBackBufferNum(const UINT a_set) { m_backBufferNum = a_set; }

	private:

		bool CreateSwapChain     (const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig);
		bool CreateRTVHeap       ();
		bool CreateBackBufferList();

		static constexpr UINT k_defaultSampleCount   = 1U;
		static constexpr UINT k_defaultSampleQuality = 0U;

		const Hardware&			    k_hardware;
		const GraphicsCommandQueue& k_graphicsCommandQueue;

		std::vector<TypeDEF::ComPtr<ID3D12Resource2>> m_backBufferList;

		TypeDEF::ComPtr<IDXGISwapChain4> m_swapChain;

		DescriptorHeap m_rtvHeap;

		Converter::SwapChainJsonConverter m_swapChainJsonConverter;

		UINT m_backBufferNum;
	};
}