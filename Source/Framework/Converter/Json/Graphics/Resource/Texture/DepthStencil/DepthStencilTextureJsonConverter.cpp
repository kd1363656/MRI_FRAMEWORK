#include "DepthStencilTextureJsonConverter.h"

void FWK::Converter::DepthStencilTextureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::DepthStencilTexture& a_depthStencilTexture) const
{
	if (a_rootJson.is_null()) { return; }
}

nlohmann::json FWK::Converter::DepthStencilTextureJsonConverter::Serialize(const Graphics::DepthStencilTexture& a_depthStencilTexture) const
{
	return nlohmann::json();
}