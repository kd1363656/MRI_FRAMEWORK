#include "Shader.h"

void FWK::Graphics::Shader::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_shaderJsonConverter.Deserialize(a_rootJson, *this);
}

bool FWK::Graphics::Shader::CreateFromFile(const ShaderCompiler& a_shaderCompiler)
{
	if (m_filePath.extension() == Constant::k_lowerHLSLExtension)
	{
		if (m_entryPointName.empty())
		{
			assert(false && "HLSLシェーダーのエントリーポイント名が空のため、シェーダー作成に失敗しました。");
			return false;
		}

		if (m_shaderModelVersionName.empty())
		{
			assert(false && "HLSLシェーダーモデルバージョン名が空のため、シェーダー作成に失敗しました。");
			return false;
		}

		m_dxcBlob = a_shaderCompiler.CompileFromFile(m_filePath.c_str(), m_entryPointName, m_shaderModelVersionName);
	}
	else if (m_filePath.extension() == Constant::k_lowerCSOExtension)
	{
		m_dxcBlob = a_shaderCompiler.LoadBinaryFromFile(m_filePath.c_str());
	}
	else 
	{
		assert(false && "シェーダーファイルの拡張子が.hlslまたは.csoではありません");
		return false;
	}

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