#include "ShaderJsonConverter.h"

void FWK::JsonConverter::ShaderJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::Shader& a_shader) const
{
	if (a_rootJson.is_null()) { return; }

		  auto	l_filePath               = a_rootJson.value("FilePath",               std::filesystem::path());
	const auto& l_entryPointName         = a_rootJson.value("EntryPointName",         std::string          ());
	const auto& l_shaderModelVersionName = a_rootJson.value("ShaderModelVersionName", std::string          ());

	a_shader.SetFilePath              (l_filePath);
	a_shader.SetEntryPointName        (l_entryPointName);
	a_shader.SetShaderModelVersionName(l_shaderModelVersionName);
}

nlohmann::json FWK::JsonConverter::ShaderJsonConverter::Serialize(const Graphics::Shader& a_shader) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_filePath               = a_shader.GetFilePath              ();
	const auto& l_entryPointName         = a_shader.GetEntryPointName        ();
	const auto& l_shaderModelVersionName = a_shader.GetShaderModelVersionName();

	l_rootJson["FilePath"]               = l_filePath;
	l_rootJson["EntryPointName"]         = l_entryPointName;
	l_rootJson["ShaderModelVersionName"] = l_shaderModelVersionName;

	return l_rootJson;
}