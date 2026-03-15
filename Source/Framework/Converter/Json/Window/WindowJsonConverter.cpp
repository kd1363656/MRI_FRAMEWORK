#include "WindowJsonConverter.h"

FWK::Converter::WindowJsonConverter::WindowJsonConverter(Window& a_window) :
	m_window(a_window)
{}
FWK::Converter::WindowJsonConverter::~WindowJsonConverter() = default;

void FWK::Converter::WindowJsonConverter::Deserialize(const nlohmann::json& a_json)
{
	if (a_json.is_null()) { return; }

	auto l_windowConfig = CommonStruct::WindowConfig();

	l_windowConfig.width    = a_json.value("Width",  CommonConstant::k_defaultWindowWidth);
	l_windowConfig.height   = a_json.value("Height", CommonConstant::k_defaultWindowHeight);
	l_windowConfig.styleTag = Utility::Json::DeserializeTag(a_json);

	m_window.SetWindowConfig(l_windowConfig);
}
nlohmann::json FWK::Converter::WindowJsonConverter::Serialize()
{
	auto        l_json         = nlohmann::json();
	const auto& l_windowConfig = m_window.GetWindowConfig();

	l_json["Width"]  = l_windowConfig.width;
	l_json["Height"] = l_windowConfig.height;
	Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_windowConfig.styleTag));

	return l_json;
}