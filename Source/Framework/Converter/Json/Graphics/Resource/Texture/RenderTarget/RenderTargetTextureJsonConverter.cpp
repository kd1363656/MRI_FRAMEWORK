#include "RenderTargetTextureJsonConverter.h"

void FWK::Converter::RenderTargetTextureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderTargetTexture& a_renderTargetTexture) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_clearColorJsonKey))
	{
		a_renderTargetTexture.SetClearColor(Utility::Json::DeserializeColor(a_rootJson[k_clearColorJsonKey]));
	}

	a_renderTargetTexture.SetFormat(a_rootJson.value(k_formatJsonKey, Constant::k_defaultSwapChainBackBufferFormat));

	a_renderTargetTexture.SetWidth			(a_rootJson.value(k_widthJsonKey,  Constant::k_invalidRenderTextureWidth));
	a_renderTargetTexture.SetHeight			(a_rootJson.value(k_heightJsonKey, Constant::k_invalidRenderTextureHeight));
	a_renderTargetTexture.SetIsUseWindowSize(a_rootJson.value(k_isUseWindowSizeJsonKey, false));
}

nlohmann::json FWK::Converter::RenderTargetTextureJsonConverter::Serialize(const Graphics::RenderTargetTexture& a_renderTargetTexture) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_clearColorJsonKey] = Utility::Json::SerializeColor(a_renderTargetTexture.GetClearColor());

	l_rootJson[k_formatJsonKey] = a_renderTargetTexture.GetFormat();

	l_rootJson[k_widthJsonKey]			 = a_renderTargetTexture.GetWidth		   ();
	l_rootJson[k_heightJsonKey]			 = a_renderTargetTexture.GetHeight		   ();
	l_rootJson[k_isUseWindowSizeJsonKey] = a_renderTargetTexture.GetIsUseWindowSize();

	return l_rootJson;
}