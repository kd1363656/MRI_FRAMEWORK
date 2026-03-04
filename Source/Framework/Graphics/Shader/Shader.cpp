#include "Shader.h"

FWK::Graphics::Shader::Shader(const ShaderCompiler& a_shaderCompiler) :
	k_shaderCompiler     (a_shaderCompiler),
	m_blob				 (nullptr),
	m_shaderJsonConverter(*this),
	m_filePath           (),
	m_entryPointName     (),
	m_shaderModelName    ()
{}
FWK::Graphics::Shader::~Shader() = default;

void FWK::Graphics::Shader::Init()
{
	m_blob.Reset();

	m_filePath.clear       ();
	m_entryPointName.clear ();
	m_shaderModelName.clear();
}
void FWK::Graphics::Shader::CompileFromFile()
{
	if (m_filePath.empty()       ||
		m_entryPointName.empty() ||
		m_shaderModelName.empty()) 
	{
		return; 
	}

	k_shaderCompiler.CompileFromFile(m_filePath, 
									 m_entryPointName,
									 m_shaderModelName,
									 m_blob);
}

void FWK::Graphics::Shader::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_shaderJsonConverter.Deserialize(a_rootJson);
}
nlohmann::json FWK::Graphics::Shader::Serialize()
{
	return m_shaderJsonConverter.Serialize();
}