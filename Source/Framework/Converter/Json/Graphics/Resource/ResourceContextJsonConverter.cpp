#include "ResourceContextJsonConverter.h"

void FWK::Converter::ResourceContextJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::ResourceContext& a_resourceContext) const
{
	if (a_rootJson.is_null()) { return; }

	// RTVディスクリプタプールのデシリアライズ
	if (a_rootJson.contains(k_rtvDescriptorPoolJsonKey))
	{
		auto& l_rtvDescriptorPool = a_resourceContext.GetMutableREFRTVDescriptorPool();

		l_rtvDescriptorPool.Deserialize(a_rootJson[k_rtvDescriptorPoolJsonKey]);
	}

	// SRVディスクリプタプールのデシリアライズ
	if (a_rootJson.contains(k_srvDescriptorPoolJsonKey))
	{
		auto& l_srvDescriptorPool = a_resourceContext.GetMutableREFSRVDescriptorPool();

		l_srvDescriptorPool.Deserialize(a_rootJson[k_srvDescriptorPoolJsonKey]);
	}

	// テクスチャシステムのデシリアライズ
	if (a_rootJson.contains(k_textureSystemJsonKey))
	{
		auto& l_textureSystem = a_resourceContext.GetMutableREFTextureSystem();

		l_textureSystem.Deserialize(a_rootJson[k_textureSystemJsonKey]);
	}

	// スタティックモデルシステムのデシリアライズ
	if (a_rootJson.contains(k_staticModelSystemJsonKey))
	{
		auto& l_staticModelSystem = a_resourceContext.GetMutableREFStaticModelSystem();

		l_staticModelSystem.Deserialize(a_rootJson[k_staticModelSystemJsonKey]);
	}

	// アップロードシステムのデシリアライズ
	if (a_rootJson.contains(k_uploadSystemJsonKey))
	{
		auto& l_uploadSystem = a_resourceContext.GetMutableREFUploadSystem();

		l_uploadSystem.Deserialize(a_rootJson[k_uploadSystemJsonKey]);
	}
}

nlohmann::json FWK::Converter::ResourceContextJsonConverter::Serialize(const Graphics::ResourceContext& a_resourceContext) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_rtvDescriptorPool = a_resourceContext.GetREFRTVDescriptorPool();
	const auto& l_srvDescriptorPool = a_resourceContext.GetREFSRVDescriptorPool();
	
	const auto& l_textureSystem     = a_resourceContext.GetREFTextureSystem    ();
	const auto& l_staticModelSystem = a_resourceContext.GetREFStaticModelSystem();
	const auto& l_uploadSystem      = a_resourceContext.GetREFUploadSystem     ();

	// RTVディスクリプタプールのシリアライズ
	l_rootJson[k_rtvDescriptorPoolJsonKey] = l_rtvDescriptorPool.Serialize();

	// SRVディスクリプタプールのシリアライズ
	l_rootJson[k_srvDescriptorPoolJsonKey] = l_srvDescriptorPool.Serialize();

	// テクスチャシステムのシリアライズ
	l_rootJson[k_textureSystemJsonKey] = l_textureSystem.Serialize();

	// スタティックモデルシステムのシリアライズ
	l_rootJson[k_staticModelSystemJsonKey] = l_staticModelSystem.Serialize();

	// アップロードシステムのシリアライズ
	l_rootJson[k_uploadSystemJsonKey] = l_uploadSystem.Serialize();

	return l_rootJson;
}