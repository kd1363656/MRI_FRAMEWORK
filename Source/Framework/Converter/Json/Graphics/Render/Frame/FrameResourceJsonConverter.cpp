#include "FrameResourceJsonConverter.h"

void FWK::Converter::FrameResourceJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::FrameResource& a_frameResource) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_renderGraphResourceRegistryJsonKey))
	{
		auto& l_renderGraphResourceRegistry = a_frameResource.GetMutableREFRenderGraphResourceRegistry();

		l_renderGraphResourceRegistry.Deserialize(a_rootJson[k_renderGraphResourceRegistryJsonKey]);
	}

	// 定数バッファのデシリアライズ
	if (a_rootJson.contains(k_constantBufferUploaderMapJsonKey))
	{
		DeserializeConstantBuffer(a_rootJson[k_constantBufferUploaderMapJsonKey], a_frameResource);
	}
}

nlohmann::json FWK::Converter::FrameResourceJsonConverter::Serialize(const Graphics::FrameResource& a_frameResource) const
{
	nlohmann::json l_rootJson  = {};

	const auto& l_renderGraphResourceRegistry = a_frameResource.GetREFRenderGraphResourceRegistry();
	
	l_rootJson[k_renderGraphResourceRegistryJsonKey] = l_renderGraphResourceRegistry.Serialize();
	l_rootJson[k_constantBufferUploaderMapJsonKey]   = SerializeConstantBuffer (a_frameResource);

	return l_rootJson;
}

void FWK::Converter::FrameResourceJsonConverter::DeserializeConstantBuffer(const nlohmann::json& a_rootJson, Graphics::FrameResource& a_frameResource) const
{
	if (!Utility::Json::IsArray(a_rootJson)) { return; }
	
	for (const auto& l_json : a_rootJson)
	{
		std::shared_ptr<Graphics::ConstantBufferUploaderBase> l_constantBufferUploader = nullptr;

		// 定数バッファクラスをデシリアライズ
		Utility::Json::DeserializeInstanceType<TypeAlias::ShaderFactoryConstantBufferUploader>(l_json, k_constantBufferUploaderTypeNameJsonKey, l_constantBufferUploader);

		// 作製に成功していれば中身にポインタがしっかり入っているので初期化とデシリアライズを行う
		if (!l_constantBufferUploader) { continue; }
		
		if (l_json.contains(k_constantBufferUploaderJsonKey))
		{
			l_constantBufferUploader->Deserialize(l_json[k_constantBufferUploaderJsonKey]);
		}

		a_frameResource.AddConstantBuffer(l_constantBufferUploader);
	}
}

nlohmann::json FWK::Converter::FrameResourceJsonConverter::SerializeConstantBuffer(const Graphics::FrameResource& a_frameResource) const
{
	nlohmann::json l_rootJsonArray = {};
	
	const auto& l_constantBufferMap = a_frameResource.GetREFConstantBufferMap();

	// 生成する定数バッファの名前とその定数バッファに必要な情報をSerialize
	for (const auto& [l_staticTypeID, l_constantBuffer] : l_constantBufferMap)
	{
		if (!l_constantBuffer) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson											 (l_json, Utility::Json::SerializeInstanceType(l_constantBuffer, k_constantBufferUploaderTypeNameJsonKey));
		l_json[k_constantBufferUploaderJsonKey] = l_constantBuffer->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}