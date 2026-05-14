#include "TextureSystemJsonConverter.h"

void FWK::Converter::TextureSystemJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::TextureSystem& a_textureSystem) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_textureStorageJsonKey))
	{
		auto& l_textureStorage = a_textureSystem.GetMutableREFTextureStorage();

		l_textureStorage.Deserialize(a_rootJson[k_textureStorageJsonKey]);
	}
	
	const auto l_storageIDAllocatorCapacity = a_rootJson.value(k_storageIDAllocatorCapacityJsonKey, Constant::k_defaultCreateStorageIDCapacity);

	a_textureSystem.SetStorageIDAllocatorCapacity(l_storageIDAllocatorCapacity);
}

nlohmann::json FWK::Converter::TextureSystemJsonConverter::Serialize(const Graphics::TextureSystem& a_textureSystem) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_textureStorage = a_textureSystem.GetREFTextureStorage();

	l_rootJson[k_textureStorageJsonKey]			    = l_textureStorage.Serialize					  ();
	l_rootJson[k_storageIDAllocatorCapacityJsonKey] = a_textureSystem.GetVALStorageIDAllocatorCapacity();

	return l_rootJson;
}