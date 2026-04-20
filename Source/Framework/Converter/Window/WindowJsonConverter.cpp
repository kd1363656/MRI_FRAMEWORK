#include "WindowJsonConverter.h"

void FWK::Converter::WindowJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Window& a_window)
{
	if (a_rootJson.is_null()) { return; }

}

nlohmann::json FWK::Converter::WindowJsonConverter::Serialize(const Window& a_window) const
{
	return nlohmann::json();
}