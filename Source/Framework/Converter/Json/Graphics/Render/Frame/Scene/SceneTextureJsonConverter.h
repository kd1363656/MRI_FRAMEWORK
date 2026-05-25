#pragma once

namespace FWK::Graphics
{
	class SceneTexture;
}

namespace FWK::Converter
{
	class SceneTextureJsonConverter final
	{
	public:

		 SceneTextureJsonConverter() = default;
		~SceneTextureJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const;

		nlohmann::json Serialize(const Graphics::SceneTexture& a_sceneTexture) const;
	};
}