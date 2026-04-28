#include "ResourceContextJsonConverter.h"

void FWK::JsonConverter::ResourceContextJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::ResourceContext& a_resourceContext) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains("RTVDescriptorPool"))
	{
		auto& l_rtvDescriptorPool = a_resourceContext.GetMutableREFRTVDescriptorPool();

		l_rtvDescriptorPool.Deserialize(a_rootJson["RTVDescriptorPool"]);
	}

	if (a_rootJson.contains("SRVDescriptorPool"))
	{
		auto& l_srvDescriptorPool = a_resourceContext.GetMutableREFSRVDescriptorPool();

		l_srvDescriptorPool.Deserialize(a_rootJson["SRVDescriptorPool"]);
	}

	if (a_rootJson.contains("UploadSystem"))
	{
		auto& l_uploadSystem = a_resourceContext.GetMutableREFUploadSystem();

		l_uploadSystem.Deserialize(a_rootJson["UploadSystem"]);
	}
}

nlohmann::json FWK::JsonConverter::ResourceContextJsonConverter::Serialize(const Graphics::ResourceContext& a_resourceContext) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_rtvDescriptorPool = a_resourceContext.GetREFRTVDescriptorPool();
	const auto& l_srvDescriptorPool = a_resourceContext.GetREFSRVDescriptorPool();
	
	const auto& l_uploadSystem = a_resourceContext.GetREFUploadSystem();

	l_rootJson["RTVDescriptorPool"] = l_rtvDescriptorPool.Serialize();
	l_rootJson["SRVDescriptorPool"] = l_srvDescriptorPool.Serialize();

	l_rootJson["UploadSystem"] = l_uploadSystem.Serialize();

	return l_rootJson;
}