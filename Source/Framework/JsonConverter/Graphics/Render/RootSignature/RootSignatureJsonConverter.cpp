#include "RootSignatureJsonConverter.h"

void FWK::JsonConverter::RootSignatureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null()) { return; }
}

nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::Serialize(const Graphics::RootSignature& a_rootSignature) const
{
	nlohmann::json l_rootJson = {};

	return l_rootJson;
}