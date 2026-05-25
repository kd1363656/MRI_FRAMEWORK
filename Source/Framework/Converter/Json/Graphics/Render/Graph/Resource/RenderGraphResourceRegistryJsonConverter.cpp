#include "RenderGraphResourceRegistryJsonConverter.h"

void FWK::Converter::RenderGraphResourceRegistryConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_renderTargetTextureRecordListJsonKey))
	{
		DeserializeRenderTargetTextureRecord(a_rootJson[k_renderTargetTextureRecordListJsonKey], a_renderGraphResourceRegistry);
	}

	if (a_rootJson.contains(k_depthStencilTextureRecordListJsonKey))
	{
		DeserializeDepthStencilTextureRecord(a_rootJson[k_depthStencilTextureRecordListJsonKey], a_renderGraphResourceRegistry);
	}
}

nlohmann::json FWK::Converter::RenderGraphResourceRegistryConverter::Serialize(const Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_renderTargetTextureRecordListJsonKey] = SerializeRenderTargetTextureRecord(a_renderGraphResourceRegistry);
	l_rootJson[k_depthStencilTextureRecordListJsonKey] = SerializeDepthStencilTextureRecord(a_renderGraphResourceRegistry);

	return l_rootJson;
}

void FWK::Converter::RenderGraphResourceRegistryConverter::DeserializeRenderTargetTextureRecord(const nlohmann::json& a_rootJson, Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		const auto l_textureTag = Utility::Json::DeserializeTag(l_json, k_renderTargetTextureTagJsonKey);

		if (l_textureTag == Constant::k_invalidTypeTag)
		{
			assert(false && "RenderGraph管理RenderTargetTexture用TextureTagの取得に失敗しました。");
			continue;
		}

		if (!l_json.contains(k_renderTargetTextureJsonKey))
		{
			assert(false && "RenderGraph管理RenderTargetTextureのJsonが存在しません。");
			continue;
		}

		auto l_renderTargetTextureResourceRecord = std::make_shared<Struct::RenderGraphRenderTargetTextureResourceRecord>();

		l_renderTargetTextureResourceRecord->m_textureTag		   = l_textureTag;
		l_renderTargetTextureResourceRecord->m_renderTargetTexture = std::make_shared<Graphics::RenderTargetTexture>();

		l_renderTargetTextureResourceRecord->m_renderTargetTexture->Deserialize(l_json[k_renderTargetTextureJsonKey]);

		a_renderGraphResourceRegistry.AddRenderTargetTexture(l_renderTargetTextureResourceRecord);
	}
}
void FWK::Converter::RenderGraphResourceRegistryConverter::DeserializeDepthStencilTextureRecord(const nlohmann::json& a_rootJson, Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		const auto l_textureTag = Utility::Json::DeserializeTag(l_json, k_depthStencilTextureJsonKey);

		if (l_textureTag == Constant::k_invalidTypeTag)
		{
			assert(false && "RenderGraph管理DepthStencilTexture用TextureTagの取得に失敗しました。");
			continue;
		}

		if (!l_json.contains(k_depthStencilTextureJsonKey))
		{
			assert(false && "RenderGraph管理DepthStencilTextureのJsonが存在しません。");
			continue;
		}

		auto l_depthStencilTextureResourceRecord = std::make_shared<Struct::RenderGraphDepthStencilTextureResourceRecord>();

		l_depthStencilTextureResourceRecord->m_textureTag		   = l_textureTag;
		l_depthStencilTextureResourceRecord->m_depthStencilTexture = std::make_shared<Graphics::DepthStencilTexture>();

		l_depthStencilTextureResourceRecord->m_depthStencilTexture->Deserialize(l_json[k_depthStencilTextureJsonKey]);

		a_renderGraphResourceRegistry.AddDepthStencilTexture(l_depthStencilTextureResourceRecord);
	}
}

nlohmann::json FWK::Converter::RenderGraphResourceRegistryConverter::SerializeRenderTargetTextureRecord(const Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	for (const auto& l_renderTargetTextureResourceRecord : a_renderGraphResourceRegistry.GetREFRenderTargetTextureResourceRecordList())
	{
		if (!l_renderTargetTextureResourceRecord) { continue; }

		const auto& l_renderTargetTexture = l_renderTargetTextureResourceRecord->m_renderTargetTexture;

		if (!l_renderTargetTexture) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_renderTargetTextureResourceRecord->m_textureTag, k_renderTargetTextureTagJsonKey));

		l_json[k_renderTargetTextureJsonKey] = l_renderTargetTexture->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::Converter::RenderGraphResourceRegistryConverter::SerializeDepthStencilTextureRecord(const Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	for (const auto& l_depthStencilTextureResourceRecord : a_renderGraphResourceRegistry.GetREFDepthStencilTextureResourceRecordList())
	{
		if (!l_depthStencilTextureResourceRecord) { continue; }

		const auto& l_depthStencilTexture = l_depthStencilTextureResourceRecord->m_depthStencilTexture;

		if (!l_depthStencilTexture) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_depthStencilTextureResourceRecord->m_textureTag, k_depthStencilTextureTagJsonKey));

		l_json[k_depthStencilTextureJsonKey] = l_depthStencilTexture->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}