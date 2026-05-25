#include "RenderTargetTextureJsonConverter.h"

void FWK::Converter::RenderTargetTextureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderTargetTexture& a_renderTargetTexture) const
{
	if (a_rootJson.is_null()) { return; }
}

nlohmann::json FWK::Converter::RenderTargetTextureJsonConverter::Serialize(const Graphics::RenderTargetTexture& a_renderTargetTexture) const
{
	return nlohmann::json();
}