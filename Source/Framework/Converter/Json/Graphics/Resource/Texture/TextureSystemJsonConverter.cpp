#include "TextureSystemJsonConverter.h"

void FWK::Converter::TextureSystemJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::TextureSystem& a_textureSystem) const
{
	if (a_rootJson.is_null()) { return; }

	const auto l_storageIDAllocatorCapacity = a_rootJson.value(k_storageIDAllocatorCapacityJsonKey, Constant::k_defaultCreateStorageIDCapacity);

	a_textureSystem.SetTextureIDAllocatorCapacity(l_storageIDAllocatorCapacity);
}

nlohmann::json FWK::Converter::TextureSystemJsonConverter::Serialize(const Graphics::TextureSystem& a_textureSystem) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_storageIDAllocatorCapacityJsonKey] = a_textureSystem.GetVALStorageIDAllocatorCapacity();

	return l_rootJson;
}