#include "GraphicsManagerJsonConverter.h"

void FWK::Converter::GraphicsManagerJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::GraphicsManager& a_graphicsManager) const
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
}

nlohmann::json FWK::Converter::GraphicsManagerJsonConverter::Serialize(const Graphics::GraphicsManager& a_graphicsManager) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_resourceContext = a_graphicsManager.GetREFResourceContext();
	const auto& l_renderer		  = a_graphicsManager.GetREFRenderer       ();

	l_rootJson["ResourceContext"] = l_resourceContext.Serialize();
	l_rootJson["Renderer"]        = l_renderer.Serialize	   ();

	return l_rootJson;
}