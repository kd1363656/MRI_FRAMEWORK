#include "GraphicsManagerJsonConverter.h"

void FWK::JsonConverter::GraphicsManagerJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::GraphicsManager& a_graphicsManager) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_resourceContextJsonKey))
	{
		auto& l_resourceContext = a_graphicsManager.GetMutableREFResourceContext();

		l_resourceContext.Deserialize(a_rootJson[k_resourceContextJsonKey]);
	}

	if (a_rootJson.contains(k_rendererJsonKey))
	{
		auto& l_renderer = a_graphicsManager.GetMutableREFRenderer();

		l_renderer.Deserialize(a_rootJson[k_rendererJsonKey]);
	}

	if (a_rootJson.contains(k_swapChainJsonKey))
	{
		auto& l_swapChain = a_graphicsManager.GetMutableREFSwapChain();

		l_swapChain.Deserialize(a_rootJson[k_swapChainJsonKey]);
	}
}

nlohmann::json FWK::JsonConverter::GraphicsManagerJsonConverter::Serialize(const Graphics::GraphicsManager& a_graphicsManager) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_resourceContext = a_graphicsManager.GetREFResourceContext();
	const auto& l_renderer		  = a_graphicsManager.GetREFRenderer       ();
	const auto& l_swapChain		  = a_graphicsManager.GetREFSwapChain	   ();

	l_rootJson[k_resourceContextJsonKey] = l_resourceContext.Serialize();
	l_rootJson[k_rendererJsonKey]        = l_renderer.Serialize	      ();
	l_rootJson[k_swapChainJsonKey]		 = l_swapChain.Serialize	  ();

	return l_rootJson;
}