#include "TextureSystemJsonConverter.h"

void FWK::Converter::TextureSystemJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::TextureSystem& a_textureSystem) const
{
	if (a_rootJson.is_null()) { return; }

	const auto l_textureIDAllocatorCapacity = a_rootJson.value(k_textureIDAllocatorCapacityJsonKey, Constant::k_defaultCreateTextureIDCapacity);

	a_textureSystem.SetTextureIDAllocatorCapacity(l_textureIDAllocatorCapacity);
}

nlohmann::json FWK::Converter::TextureSystemJsonConverter::Serialize(const Graphics::TextureSystem& a_textureSystem) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_textureIDAllocatorCapacityJsonKey] = a_textureSystem.GetVALTextureIDAllocatorCapacity();

	return l_rootJson;
}