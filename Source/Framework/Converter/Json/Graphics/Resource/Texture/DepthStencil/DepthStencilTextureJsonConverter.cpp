#include "DepthStencilTextureJsonConverter.h"

void FWK::Converter::DepthStencilTextureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::DepthStencilTexture& a_depthStencilTexture) const
{
	if (a_rootJson.is_null()) { return; }

	a_depthStencilTexture.SetWidth (a_rootJson.value(k_widthJsonKey,  Constant::k_defaultDepthStencilTextureWidth));
	a_depthStencilTexture.SetHeight(a_rootJson.value(k_heightJsonKey, Constant::k_defaultDepthStencilTextureHeight));
}

nlohmann::json FWK::Converter::DepthStencilTextureJsonConverter::Serialize(const Graphics::DepthStencilTexture& a_depthStencilTexture) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_widthJsonKey]  = a_depthStencilTexture.GetWidth ();
	l_rootJson[k_heightJsonKey] = a_depthStencilTexture.GetHeight();

	return l_rootJson;
}