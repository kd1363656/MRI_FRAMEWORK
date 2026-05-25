#include "SceneTextureJsonConverter.h"

void FWK::Converter::SceneTextureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_renderTargetTextureRecordListJsonKey))
	{
		DeserializeRenderTargetTextureRecord(a_rootJson[k_renderTargetTextureRecordListJsonKey], a_sceneTexture);
	}

	if (a_rootJson.contains(k_depthStencilTextureRecordListJsonKey))
	{
		DeserializeDepthStencilTextureRecord(a_rootJson[k_depthStencilTextureRecordListJsonKey], a_sceneTexture);
	}
}

nlohmann::json FWK::Converter::SceneTextureJsonConverter::Serialize(const Graphics::SceneTexture& a_sceneTexture) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_renderTargetTextureRecordListJsonKey] = SerializeRenderTargetTextureRecord(a_sceneTexture);
	l_rootJson[k_depthStencilTextureRecordListJsonKey] = SerializeDepthStencilTextureRecord(a_sceneTexture);

	return l_rootJson;
}

void FWK::Converter::SceneTextureJsonConverter::DeserializeRenderTargetTextureRecord(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		const auto l_tag = Utility::Json::DeserializeTag(l_json, k_renderTargetTextureTagJsonKey);

		if (l_tag == Constant::k_invalidTypeTag)
		{
			assert(false && "RenderTargetTexture用TypeTagの取得に失敗しました。");
			continue;
		}

		if (!l_json.contains(k_renderTargetTextureJsonKey))
		{
			assert(false && "RenderTargetTextureのJsonが存在しません。");
			continue;
		}

		Struct::SceneRenderTargetTextureRecord l_sceneRenderTargetTextureRecord = {};
		
		auto& l_renderTargetTexture = l_sceneRenderTargetTextureRecord.m_renderTargetTexture;

		l_renderTargetTexture				       = std::make_shared<Graphics::RenderTargetTexture>();
		l_sceneRenderTargetTextureRecord.m_typeTag = l_tag;

		l_sceneRenderTargetTextureRecord.m_renderTargetTexture->Deserialize(l_json[k_renderTargetTextureJsonKey]);

		a_sceneTexture.AddRenderTargetTexture(l_sceneRenderTargetTextureRecord);
	}
}
void FWK::Converter::SceneTextureJsonConverter::DeserializeDepthStencilTextureRecord(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		const auto l_tag = Utility::Json::DeserializeTag(l_json, k_depthStencilTextureTagJsonKey);

		if (l_tag == Constant::k_invalidTypeTag)
		{
			assert(false && "DepthStencilTexture用TypeTagの取得に失敗しました。");
			continue;
		}
		
		if (!l_json.contains(k_depthStencilTextureJsonKey))
		{
			assert(false && "DepthStencilTextureのJsonが存在しません。");
			continue;
		}

		Struct::SceneDepthStencilTextureRecord l_sceneDepthStencilTextureRecord = {};
		
		auto& l_depthStencilTexture = l_sceneDepthStencilTextureRecord.m_depthStencilTexture;

		l_depthStencilTexture					   = std::make_shared<Graphics::DepthStencilTexture>();
		l_sceneDepthStencilTextureRecord.m_typeTag = l_tag;

		l_sceneDepthStencilTextureRecord.m_depthStencilTexture->Deserialize(l_json[k_depthStencilTextureJsonKey]);

		a_sceneTexture.AddDepthStencilTexture(l_sceneDepthStencilTextureRecord);
	}
}

nlohmann::json FWK::Converter::SceneTextureJsonConverter::SerializeRenderTargetTextureRecord(const Graphics::SceneTexture& a_sceneTexture) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	for (const auto& l_sceneRenderTargetTextureRecord : a_sceneTexture.GetRenderTargetTextureRecordList())
	{
		auto& l_renderTargetTexture = l_sceneRenderTargetTextureRecord.m_renderTargetTexture;

		if (!l_renderTargetTexture) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_sceneRenderTargetTextureRecord.m_typeTag, k_renderTargetTextureTagJsonKey));
		l_json[k_renderTargetTextureJsonKey] = l_renderTargetTexture->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::Converter::SceneTextureJsonConverter::SerializeDepthStencilTextureRecord(const Graphics::SceneTexture& a_sceneTexture) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	for (const auto& l_sceneDepthStencilTextureRecord : a_sceneTexture.GetDepthStencilTextureRecordList())
	{
		auto& l_depthStencilTexture = l_sceneDepthStencilTextureRecord.m_depthStencilTexture;

		if (!l_depthStencilTexture) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_sceneDepthStencilTextureRecord.m_typeTag, k_depthStencilTextureTagJsonKey));
		l_json[k_depthStencilTextureJsonKey] = l_depthStencilTexture->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}