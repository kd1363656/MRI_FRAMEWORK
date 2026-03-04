#pragma once

namespace FWK::Graphics
{
	class Shader
	{
	public:

		Shader (const ShaderCompiler& a_shaderCompiler);
		~Shader();

		void Init           ();
		void CompileFromFile();
		
		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize  ();

		const auto& GetBlob() const { return m_blob; }

		const auto& GetFilePath       () const { return m_filePath; }
		const auto& GetEntryPointName () const { return m_entryPointName; }
		const auto& GetShaderModelName() const { return m_shaderModelName; }

		void SetFilePath       (const std::string& a_set) { m_filePath        = a_set; }
		void SetEntryPointName (const std::string& a_set) { m_entryPointName  = a_set; }
		void SetShaderModelName(const std::string& a_set) { m_shaderModelName = a_set; }

	private:

		const ShaderCompiler& k_shaderCompiler;

		ComPtr<IDxcBlob> m_blob;

		Converter::ShaderJsonConverter m_shaderJsonConverter;

		std::string m_filePath;
		std::string m_entryPointName;
		std::string m_shaderModelName;
	};
}