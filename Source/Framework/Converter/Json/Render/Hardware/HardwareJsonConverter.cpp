#include "HardwareJsonConverter.h"

FWK::Converter::HardwareJsonConverter::HardwareJsonConverter(Render::Hardware& a_hardware) : 
	m_hardware(a_hardware)
{}
FWK::Converter::HardwareJsonConverter::~HardwareJsonConverter() = default;

void FWK::Converter::HardwareJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	const UINT l_gpuNodeMask = a_rootJson.value("GPUNodeMask", CommonConstant::k_defaultGPUNodeMask);

	m_hardware.SetGPUNodeMask(l_gpuNodeMask);
}
nlohmann::json FWK::Converter::HardwareJsonConverter::Serialize()
{
	auto l_rootJson = nlohmann::json{};

	l_rootJson["GPUNodeMask"] = m_hardware.GetGPUNodeMask();

	return l_rootJson;
}