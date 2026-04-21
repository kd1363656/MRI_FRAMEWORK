#include "WindowJsonConverter.h"

void FWK::Converter::WindowJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Window& a_window) const
{
	if (a_rootJson.is_null()) { return; }

	Struct::WindowCONFIG l_windowConfig = {};

	l_windowConfig.m_width    = a_rootJson.value             ("Width",    Constant::k_defaultWindowWidth);
	l_windowConfig.m_height   =	a_rootJson.value             ("Height",   Constant::k_defaultWindowHeight);
	l_windowConfig.m_styleTag = Utility::Json::DeserializeTag(a_rootJson, "WindowStyleTag");

	a_window.SetWindowConfig(l_windowConfig);
}

nlohmann::json FWK::Converter::WindowJsonConverter::Serialize(const Window& a_window) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_windowConfig = a_window.GetREFWindowCONFIG();

	l_rootJson["Width"]  = l_windowConfig.m_width;
	l_rootJson["Height"] = l_windowConfig.m_height;
	Utility::Json::UpdateJson(l_rootJson, Utility::Json::SerializeTag(l_windowConfig.m_styleTag, "WindowStyleTag"));

	return l_rootJson;
}