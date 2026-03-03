#pragma once

namespace FWK::Graphics
{
	class SwapChain final
	{
	public:

		explicit SwapChain(const Hardware& a_hardware, const RTVDescriptorHeap& a_rtvDescriptorHeap);
		~SwapChain        ();

		void Init          ();
		bool Create        (const HWND&						  a_hWND,
					        const CommonStruct::WindowConfig& a_windowConfig,
					        const DirectCommandQueue&		  a_directCommandQueue,
					        const RTVDescriptorHeap&		  a_rtvDescriptorHeap);
		void PostCreateInit(const HWND& a_hWND) const;

		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize  ();

		const auto& GetBackBufferList() const { return m_backBufferList; }

		const auto& GetSwapChain() const { return m_swapChain; }

		UINT GetBackBufferNum         () const { return m_backBufferNum; }
		UINT GetCurrentBackBufferIndex() const;

		void SetBackBufferNum(const UINT a_set) { m_backBufferNum = a_set; }

	private:

		bool CreateSwapChain     (const HWND&			   a_hWND, const CommonStruct::WindowConfig& a_windowConfig, const DirectCommandQueue& a_directCommandQueue);
		bool CreateBackBufferList(const RTVDescriptorHeap& a_rtvDescriptorHeap);

		static constexpr UINT k_defaultSampleCount   = 1U;
		static constexpr UINT k_defaultSampleQuality = 0U;

		const Hardware& k_hardware;
		
		std::vector<ComPtr<ID3D12Resource2>> m_backBufferList;

		ComPtr<IDXGISwapChain4> m_swapChain;

		Converter::SwapChainJsonConverter m_swapChainJsonConverter;

		UINT m_backBufferNum;
		UINT m_syncInterval;
	};
}