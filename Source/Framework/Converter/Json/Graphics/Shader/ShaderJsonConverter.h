#pragma once

namespace FWK::Graphics
{
	class Shader;
}

namespace FWK::Converter
{
	class ShaderJsonConverter final : public JsonConverterBase
	{
	public:

		ShaderJsonConverter (Graphics::Shader& a_shader);
		~ShaderJsonConverter();

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()								 override;

	private:

		Graphics::Shader& m_shader;
	};
}