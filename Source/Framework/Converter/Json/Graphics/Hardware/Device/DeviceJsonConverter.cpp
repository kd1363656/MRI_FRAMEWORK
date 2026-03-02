#include "DeviceJsonConverter.h"

FWK::Converter::DeviceJsonConverter::DeviceJsonConverter(Graphics::Device& a_deivce) : 
	m_device(a_deivce)
{}
FWK::Converter::DeviceJsonConverter::~DeviceJsonConverter() = default;

void FWK::Converter::DeviceJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	const UINT l_gpuNodeMask = a_rootJson.value("GPUNodeMask", CommonConstant::k_defaultGPUNodeMask);

	m_device.SetGPUNodeMask(l_gpuNodeMask);
}
nlohmann::json FWK::Converter::DeviceJsonConverter::Serialize()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["GPUNodeMask"] = m_device.GetGPUNodeMask();

	return l_rootJson;
}