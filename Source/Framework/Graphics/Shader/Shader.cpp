#include "Shader.h"

void FWK::Graphics::Shader::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_shaderJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::Shader::CreateFromFile(const ShaderCompiler& a_shaderCompiler)
{
	if (!Utility::File::CanLoadFilePath(m_filePath, Constant::k_lowerCSOExtension))
	{
		assert(false && "シェーダーファイルの拡張子が.csoではありません");
		return false;
	}

	m_dxcBlob = a_shaderCompiler.LoadBinaryFromFile(m_filePath.wstring());

	if (!m_dxcBlob)
	{
		assert(false && "シェーダーバイトコードの作成に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::Shader::Serialize() const
{
	return m_shaderJsonConverter.Serialize(*this);
}