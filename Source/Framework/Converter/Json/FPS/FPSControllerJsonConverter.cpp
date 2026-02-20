#include "FPSControllerJsonConverter.h"

FWK::Converter::FPSControllerJsonConverter::FPSControllerJsonConverter(FPSController& a_fpsController) : 
	m_fpsController(a_fpsController)
{}
FWK::Converter::FPSControllerJsonConverter::~FPSControllerJsonConverter() = default;

void FWK::Converter::FPSControllerJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	const float l_targetFPS = a_rootJson.value("TargetFPS", CommonConstant::k_defaultFPS);

	m_fpsController.SetTargetFPS(l_targetFPS);
}
nlohmann::json FWK::Converter::FPSControllerJsonConverter::Serialize()
{
	auto l_rootJson = nlohmann::json{};

	l_rootJson["TargetFPS"] = m_fpsController.GetTargetFPS();

	return l_rootJson;
}