#include "GraphicsManagerJsonConverter.h"

void FWK::Converter::GraphicsManagerJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::GraphicsManager& a_graphicsManager) const
{
	if (a_rootJson.is_null()) { return; }

	// リソースコンテキストのデシリアライズ
	if (a_rootJson.contains(k_resourceContextJsonKey))
	{
		auto& l_resourceContext = a_graphicsManager.GetMutableREFResourceContext();

		l_resourceContext.Deserialize(a_rootJson[k_resourceContextJsonKey]);
	}

	// レンダラーのデシリアライズ
	if (a_rootJson.contains(k_rendererJsonKey))
	{
		auto& l_renderer = a_graphicsManager.GetMutableREFRenderer();

		l_renderer.Deserialize(a_rootJson[k_rendererJsonKey]);
	}

	// スワップチェインのデシリアライズ
	if (a_rootJson.contains(k_swapChainJsonKey))
	{
		auto& l_swapChain = a_graphicsManager.GetMutableREFSwapChain();

		l_swapChain.Deserialize(a_rootJson[k_swapChainJsonKey]);
	}
}

nlohmann::json FWK::Converter::GraphicsManagerJsonConverter::Serialize(const Graphics::GraphicsManager& a_graphicsManager) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_resourceContext = a_graphicsManager.GetREFResourceContext();
	const auto& l_renderer		  = a_graphicsManager.GetREFRenderer       ();
	const auto& l_swapChain		  = a_graphicsManager.GetREFSwapChain	   ();

	// リソースコンテキストのシリアライズ
	l_rootJson[k_resourceContextJsonKey] = l_resourceContext.Serialize();

	// レンダラーのシリアライズ
	l_rootJson[k_rendererJsonKey] = l_renderer.Serialize();

	// スワップチェインのシリアライズ
	l_rootJson[k_swapChainJsonKey] = l_swapChain.Serialize();

	return l_rootJson;
}