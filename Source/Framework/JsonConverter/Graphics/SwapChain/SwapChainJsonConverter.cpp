#include "SwapChainJsonConverter.h"

void FWK::JsonConverter::SwapChainJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::SwapChain& a_swapChain) const
{
	if (a_rootJson.is_null()) { return; }

	const auto& l_backBufferNUM = a_rootJson.value("BackBufferNUM", static_cast<std::size_t>(Constant::k_defaultBackBufferNUM));

	a_swapChain.ResizeBackBufferList(l_backBufferNUM);
}

nlohmann::json FWK::JsonConverter::SwapChainJsonConverter::Serialize(const Graphics::SwapChain& a_swapChain) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson["BackBufferNum"] = a_swapChain.GetBackBufferList().size();

	return l_rootJson;
}