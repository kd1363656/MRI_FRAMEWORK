#include "ShaderJsonConverter.h"

FWK::Converter::ShaderJsonConverter::ShaderJsonConverter(Graphics::Shader& a_shader) :
	m_shader(a_shader)
{}
FWK::Converter::ShaderJsonConverter::~ShaderJsonConverter() = default;

void FWK::Converter::ShaderJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	const auto& l_filePath        = a_rootJson.value("FilePath"       , std::string());
	const auto& l_entryPointName  = a_rootJson.value("EntryPointName" , std::string());
	const auto& l_shaderModelName = a_rootJson.value("ShaderModelName", std::string());

	m_shader.SetFilePath       (l_filePath);
	m_shader.SetEntryPointName (l_entryPointName);
	m_shader.SetShaderModelName(l_shaderModelName);
}
nlohmann::json FWK::Converter::ShaderJsonConverter::Serialize()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["FilePath"]        = m_shader.GetFilePath       ();
	l_rootJson["EntryPointName"]  = m_shader.GetEntryPointName ();
	l_rootJson["ShaderModelName"] = m_shader.GetShaderModelName();

	return l_rootJson;
}