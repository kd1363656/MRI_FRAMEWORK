#include "RootSignature.h"

void FWK::Graphics::RootSignature::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_rootSignatureJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::RootSignature::Create(const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずルートシグネチャを作成できませんでした。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::RootSignature::Serialize()
{
	return m_rootSignatureJsonConverter.Serialize(*this);
}