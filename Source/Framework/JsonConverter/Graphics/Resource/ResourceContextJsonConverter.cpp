#include "ResourceContextJsonConverter.h"

void FWK::JsonConverter::ResourceContextJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::ResourceContext& a_resourceContext) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_rtvDescriptorPoolJsonKey))
	{
		auto& l_rtvDescriptorPool = a_resourceContext.GetMutableREFRTVDescriptorPool();

		l_rtvDescriptorPool.Deserialize(a_rootJson[k_rtvDescriptorPoolJsonKey]);
	}

	if (a_rootJson.contains(k_srvDescriptorPoolJsonKey))
	{
		auto& l_srvDescriptorPool = a_resourceContext.GetMutableREFSRVDescriptorPool();

		l_srvDescriptorPool.Deserialize(a_rootJson[k_srvDescriptorPoolJsonKey]);
	}

	if (a_rootJson.contains(k_uploadSystemJsonKey))
	{
		auto& l_uploadSystem = a_resourceContext.GetMutableREFUploadSystem();

		l_uploadSystem.Deserialize(a_rootJson[k_uploadSystemJsonKey]);
	}
}

nlohmann::json FWK::JsonConverter::ResourceContextJsonConverter::Serialize(const Graphics::ResourceContext& a_resourceContext) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_rtvDescriptorPool = a_resourceContext.GetREFRTVDescriptorPool();
	const auto& l_srvDescriptorPool = a_resourceContext.GetREFSRVDescriptorPool();
	
	const auto& l_uploadSystem = a_resourceContext.GetREFUploadSystem();

	l_rootJson[k_rtvDescriptorPoolJsonKey] = l_rtvDescriptorPool.Serialize();
	l_rootJson[k_srvDescriptorPoolJsonKey] = l_srvDescriptorPool.Serialize();

	l_rootJson[k_uploadSystemJsonKey] = l_uploadSystem.Serialize();

	return l_rootJson;
}