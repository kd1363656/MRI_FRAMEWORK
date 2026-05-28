#include "ConstantBufferUploaderJsonConverter.h"

void FWK::Converter::ConstantBufferUploaderBaseJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::ConstantBufferUploaderBase& a_constantBufferUploaderBase) const
{
	if (a_rootJson.is_null()) { return; }

	const auto& l_createConstantBuffer = a_rootJson.value(k_createConstantBufferNUMJsonKey, Constant::k_invalidCreateConstantBufferNUM);

	a_constantBufferUploaderBase.SetCreateConstantBufferNUM(l_createConstantBuffer);
}

nlohmann::json FWK::Converter::ConstantBufferUploaderBaseJsonConverter::Serialize(const Graphics::ConstantBufferUploaderBase& a_constantBufferUploaderBase) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_createConstantBufferNUMJsonKey] = a_constantBufferUploaderBase.GetREFCreateConstantBufferNUM();

	return l_rootJson;
}