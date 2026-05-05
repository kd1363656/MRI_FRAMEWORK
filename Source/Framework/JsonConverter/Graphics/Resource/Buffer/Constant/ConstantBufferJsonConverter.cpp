#include "ConstantBufferJsonConverter.h"

void FWK::JsonConverter::ConstantBufferBaseJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::ConstantBufferBase& a_constantBufferBase) const
{
	if (a_rootJson.is_null()) { return; }

	const auto& l_createConstantBuffer = a_rootJson.value("CreateConstantBuffer", Constant::k_invalidCreateConstantBufferNUM);

	a_constantBufferBase.SetCreateConstantBufferNUM(l_createConstantBuffer);
}

nlohmann::json FWK::JsonConverter::ConstantBufferBaseJsonConverter::Serialize(const Graphics::ConstantBufferBase& a_constantBufferBase) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson["CreateConstantBufferNUM"] = a_constantBufferBase.GetREFCreateConstantBufferNUM();

	return l_rootJson;
}