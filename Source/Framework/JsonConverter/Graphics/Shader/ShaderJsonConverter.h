#pragma once

namespace FWK::Graphics
{
	class Shader;
}

namespace FWK::JsonConverter
{
	class ShaderJsonConverter final
	{
	public:

		 ShaderJsonConverter() = default;
		~ShaderJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::Shader& a_shader) const;

		nlohmann::json Serialize(const Graphics::Shader& a_shader) const;
	};
}