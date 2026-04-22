#pragma once

namespace FWK::Graphics
{
	class SwapChain;
}

namespace FWK::JsonConverter
{
	class SwapChainJsonConverter final
	{
	public:

		 SwapChainJsonConverter() = default;
		~SwapChainJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::SwapChain& a_swapChain) const;

		nlohmann::json Serialize(const Graphics::SwapChain& a_swapChain) const;
	};
}