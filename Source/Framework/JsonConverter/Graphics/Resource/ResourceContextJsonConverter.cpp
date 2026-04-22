#include "ResourceContextJsonConverter.h"

void FWK::JsonConverter::ResourceContextJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::ResourceContext& a_resourceContext) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains("RTVDescriptorPool"))
	{
		auto& l_rtvDescriptorPool = a_resourceContext.GetREFMutableRTVDescriptorPool();

		l_rtvDescriptorPool.Deserialize(a_rootJson["RTVDescriptorPool"]);
	}
}

nlohmann::json FWK::JsonConverter::ResourceContextJsonConverter::Serialize(const Graphics::ResourceContext& a_resourceContext) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_rtvDescriptorPool = a_resourceContext.GetREFRTVDescriptorPool();

	l_rootJson["RTVDescriptorPool"] = l_rtvDescriptorPool.Serialize();

	return l_rootJson;
}