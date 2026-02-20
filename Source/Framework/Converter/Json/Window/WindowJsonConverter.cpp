#include "WindowJsonConverter.h"

FWK::Converter::WindowJsonConverter::WindowJsonConverter(Window& a_window) : 
	m_window(a_window)
{}
FWK::Converter::WindowJsonConverter::~WindowJsonConverter() = default;

void FWK::Converter::WindowJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	FWK::CommonStruct::WindowConfig l_windowConfig = {};

	l_windowConfig.width    = a_rootJson.value                  ("Width",  CommonConstant::k_defaultWindowWidth);
	l_windowConfig.height   = a_rootJson.value                  ("Height", CommonConstant::k_defaultWindowHeight);
	l_windowConfig.styleTag = FWK::Utility::Json::DeserializeTag(a_rootJson);

	m_window.SetWindowConfig(l_windowConfig);
}
nlohmann::json FWK::Converter::WindowJsonConverter::Serialize()
{
	auto		l_rootJson     = nlohmann::json          {};
	const auto& l_windowConfig = m_window.GetWindowConfig();

	l_rootJson["Width"]  = l_windowConfig.width;
	l_rootJson["Height"] = l_windowConfig.height;
	FWK::Utility::Json::UpdateJson(l_rootJson, FWK::Utility::Json::SerializeTag(l_windowConfig.styleTag));

	return l_rootJson;
}