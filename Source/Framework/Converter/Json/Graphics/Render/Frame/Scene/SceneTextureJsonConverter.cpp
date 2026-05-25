#include "SceneTextureJsonConverter.h"

void FWK::Converter::SceneTextureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const
{
}

nlohmann::json FWK::Converter::SceneTextureJsonConverter::Serialize(const Graphics::SceneTexture& a_sceneTexture) const
{
	return nlohmann::json();
}