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
					const Factory&                           a_factory,
					const DirectCommandQueue&                a_directCommandQueue,
					const Struct::WindowCONFIG&              a_windowCONFIG,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		nlohmann::json Serialize() const;

		void ResizeBackBufferList(const std::size_t a_backBufferNum);

		const auto& GetBackBufferList() const { return m_backBufferList; }

	private:

		bool CreateSwapChain(const HWND&                 a_hwnd,
							 const Factory&              a_factory,
							 const DirectCommandQueue&   a_directCommandQueue,
							 const Struct::WindowCONFIG& a_windowConfig);

		static constexpr DXGI_FORMAT k_swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		static constexpr UINT k_defaultSampleCount       = 1U;
		static constexpr UINT k_defaultSampleQuality     = 0U;

		TypeAlias::ComPtr<IDXGISwapChain4> m_swapChain = nullptr;

		JsonConverter::SwapChainJsonConverter m_swapChainJsonConverter = {};

		std::vector<Struct::BackBuffer> m_backBufferList = {};
	};
}