#include "SwapChainJsonConverter.h"

void FWK::JsonConverter::SwapChainJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::SwapChain& a_swapChain) const
{
	if (a_rootJson.is_null()) { return; }

	const auto& l_backBufferNUM       = a_rootJson.value("BackBufferNUM", static_cast<std::size_t>(Constant::k_defaultBackBufferNUM));
	const auto  l_defaultSyncInterval = a_rootJson.value("SyncInterval",  Constant::k_defaultSyncInterval);

	a_swapChain.ResizeBackBufferList(l_backBufferNUM);
	a_swapChain.SetSyncInterval     (l_defaultSyncInterval);
}

nlohmann::json FWK::JsonConverter::SwapChainJsonConverter::Serialize(const Graphics::SwapChain& a_swapChain) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson["BackBufferNum"] = a_swapChain.GetBackBufferList().size();
	l_rootJson["SyncInterval"]  = a_swapChain.GetSyncInterval();

	return l_rootJson;
}