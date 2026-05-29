#include "WindowJsonConverter.h"

void FWK::Converter::WindowJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Window& a_window) const
{
	if (a_rootJson.is_null()) { return; }

	Struct::WindowCONFIG l_windowCONFIG = {};

	// タグとウィンドウの幅、高さをデシリアライズ
	l_windowCONFIG.m_styleTag			 = Utility::Json::DeserializeTag(a_rootJson,      k_windowStyleTagJsonKey);
	l_windowCONFIG.m_clientSize.m_width  = a_rootJson.value             (k_widthJsonKey,  Constant::k_defaultWindowWidth);
	l_windowCONFIG.m_clientSize.m_height = a_rootJson.value             (k_heightJsonKey, Constant::k_defaultWindowHeight);

	a_window.SetWindowCONFIG(l_windowCONFIG);
}

nlohmann::json FWK::Converter::WindowJsonConverter::Serialize(const Window& a_window) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_windowConfig = a_window.GetREFWindowCONFIG();

	// タグとウィンドウの幅、高さをシリアライズ
	Utility::Json::UpdateJson(l_rootJson, Utility::Json::SerializeTag(l_windowConfig.m_styleTag, k_windowStyleTagJsonKey));
	l_rootJson[k_widthJsonKey]  = l_windowConfig.m_clientSize.m_width;
	l_rootJson[k_heightJsonKey] = l_windowConfig.m_clientSize.m_height;

	return l_rootJson;
}