#include "Shader.h"

void FWK::Graphics::Shader::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_shaderJsonConverter.Deserialize(a_rootJson, *this);
}

void FWK::Graphics::Shader::CompileFromFile(const ShaderCompiler& a_shaderCompiler)
{
	if (m_filePath.extension() != Constant::k_lowerHLSLExtension)
	{
		assert(false && "HLSLファイルの拡張子が.hlslではありません、もし.HLSLなら小文字にしてください。");
		return;
	}

	m_dxcBlob = a_shaderCompiler.CompileFromFile(m_filePath,
												 m_entryPointName,
												 m_shaderModelVersionName);
}

nlohmann::json FWK::Graphics::Shader::Serialize() const
{
	return m_shaderJsonConverter.Serialize(*this);
}