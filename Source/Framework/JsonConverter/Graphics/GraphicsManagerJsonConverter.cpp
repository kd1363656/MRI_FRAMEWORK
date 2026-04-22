#include "GraphicsManagerJsonConverter.h"

void FWK::JsonConverter::GraphicsManagerJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::GraphicsManager& a_graphicsManager) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains("ResourceContext"))
	{
		auto& l_resourceContext = a_graphicsManager.GetREFMutableResourceContext();

		l_resourceContext.Deserialize(a_rootJson["ResourceContext"]);
	}

	if (a_rootJson.contains("Renderer"))
	{
		auto& l_renderer = a_graphicsManager.GetREFMutableRenderer();

		l_renderer.Deserialize(a_rootJson["Renderer"]);
	}

	if (a_rootJson.contains("SwapChain"))
	{
		auto& l_swapChain = a_graphicsManager.GetREFMutableSwapChain();

		l_swapChain.Deserialize(a_rootJson["SwapChain"]);
	}
}

nlohmann::json FWK::JsonConverter::GraphicsManagerJsonConverter::Serialize(const Graphics::GraphicsManager& a_graphicsManager) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_resourceContext = a_graphicsManager.GetREFResourceContext();
	const auto& l_renderer		  = a_graphicsManager.GetREFRenderer       ();
	const auto& l_swapChain		  = a_graphicsManager.GetREFSwapChain	   ();

	l_rootJson["ResourceContext"] = l_resourceContext.Serialize();
	l_rootJson["Renderer"]        = l_renderer.Serialize	   ();
	l_rootJson["SwapChain"]		  = l_swapChain.Serialize	   ();

	return l_rootJson;
}