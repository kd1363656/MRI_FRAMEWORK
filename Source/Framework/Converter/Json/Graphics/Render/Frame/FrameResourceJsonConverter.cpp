#include "FrameResourceJsonConverter.h"

void FWK::Converter::FrameResourceJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::FrameResource& a_frameResource) const
{
	if (!Utility::Json::IsArray(a_rootJson, k_constantBufferMapJsonKey)) { return; }
	
	for (const auto& l_json : a_rootJson[k_constantBufferMapJsonKey])
	{
		std::shared_ptr<Graphics::ConstantBufferBase> l_constantBuffer = nullptr;

		// 定数バッファクラスをデシリアライズ
		Utility::Json::DeserializeInstanceType<TypeAlias::ShaderFactoryConstantBuffer>(l_json, k_constantBufferTypeNameJsonKey, l_constantBuffer);

		// 作製に成功していれば中身にポインタがしっかり入っているので初期化とデシリアライズを行う
		if (!l_constantBuffer) { continue; }
		
		if (l_json.contains(k_constantBufferJsonKey))
		{
			l_constantBuffer->Deserialize(l_json[k_constantBufferJsonKey]);
		}

		a_frameResource.AddConstantBuffer(l_constantBuffer);
	}
}

nlohmann::json FWK::Converter::FrameResourceJsonConverter::Serialize(const Graphics::FrameResource& a_frameResource) const
{
	nlohmann::json l_rootJson  = {};
	auto		   l_jsonArray = nlohmann::json::array();

	const auto& l_constantBufferMap = a_frameResource.GetREFConstantBufferMap();

	// 生成する定数バッファの名前とその定数バッファに必要な情報をSerialize
	for (const auto& [l_staticTypeID, l_constantBuffer] : l_constantBufferMap)
	{
		if (!l_constantBuffer) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson								     (l_json, Utility::Json::SerializeInstanceType(l_constantBuffer, k_constantBufferTypeNameJsonKey));
		l_json[k_constantBufferJsonKey] = l_constantBuffer->Serialize();

		l_jsonArray.emplace_back(l_json);
	}

	l_rootJson[k_constantBufferMapJsonKey] = l_jsonArray;

	return l_rootJson;
}