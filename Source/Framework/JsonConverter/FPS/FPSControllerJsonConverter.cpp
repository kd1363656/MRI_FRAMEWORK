#include "FPSControllerJsonConverter.h"

void FWK::JsonConverter::FPSControllerJsonConverter::Deserialize(const nlohmann::json& a_rootJson, FPSController& a_fpsController) const
{
	if (a_rootJson.is_null()) { return; }

	const float l_targetFPS = a_rootJson.value(k_targetFPSJsonKey, Constant::k_defaultFPS);

	a_fpsController.SetTargetFPS(l_targetFPS);
}

nlohmann::json FWK::JsonConverter::FPSControllerJsonConverter::Serialize(const FPSController& a_fpsController) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_targetFPSJsonKey] = a_fpsController.GetVALTargetFPS();

	return l_rootJson;
}