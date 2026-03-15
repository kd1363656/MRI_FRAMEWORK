#include "FPSControllerJsonConverter.h"

FWK::Converter::FPSControllerJsonConverter::FPSControllerJsonConverter(FPSController& a_fpsController) :
	m_fpsController(a_fpsController)
{}
FWK::Converter::FPSControllerJsonConverter::~FPSControllerJsonConverter() = default;

void FWK::Converter::FPSControllerJsonConverter::Deserialize(const nlohmann::json& a_json)
{
	if (a_json.is_null()) { return; }

	const float l_targetFPS = a_json.value("TargetFPS", CommonConstant::k_defaultFPS);

	m_fpsController.SetTargetFPS(l_targetFPS);
}
nlohmann::json FWK::Converter::FPSControllerJsonConverter::Serialize()
{
	auto l_json = nlohmann::json();

	l_json["TargetFPS"] = m_fpsController.GetTargetFPS();

	return l_json;
}