#include "WindowJsonConverter.h"

void FWK::JsonConverter::WindowJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Window& a_window) const
{
	if (a_rootJson.is_null()) { return; }

	Struct::WindowCONFIG l_windowConfig = {};

	l_windowConfig.m_styleTag = Utility::Json::DeserializeTag(a_rootJson,      k_windowStyleTagJsonKey);
	l_windowConfig.m_width    = a_rootJson.value             (k_widthJsonKey,  Constant::k_defaultWindowWidth);
	l_windowConfig.m_height   =	a_rootJson.value             (k_heightJsonKey, Constant::k_defaultWindowHeight);

	a_window.SetWindowConfig(l_windowConfig);
}

nlohmann::json FWK::JsonConverter::WindowJsonConverter::Serialize(const Window& a_window) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_windowConfig = a_window.GetREFWindowCONFIG();

	Utility::Json::UpdateJson(l_rootJson, Utility::Json::SerializeTag(l_windowConfig.m_styleTag, k_windowStyleTagJsonKey));
	l_rootJson[k_widthJsonKey]  = l_windowConfig.m_width;
	l_rootJson[k_heightJsonKey] = l_windowConfig.m_height;

	return l_rootJson;
}