#pragma once

namespace FWK::Graphics
{
	class Shader final
	{
	public:

		 Shader() = default;
		~Shader() = default;

		void Deserialize    (const nlohmann::json& a_rootJson);
		void CompileFromFile(const ShaderCompiler& a_shaderCompiler);

		nlohmann::json Serialize() const;

		void SetFilePath              (const std::filesystem::path& a_set) { m_filePath               = a_set; }
		void SetEntryPointName        (const std::string&			a_set) { m_entryPointName         = a_set; }
		void SetShaderModelVersionName(const std::string&			a_set) { m_shaderModelVersionName = a_set; }

		const auto& GetDXCBlob() const { return m_dxcBlob; }

		const auto& GetFilePath              () const { return m_filePath; }
		const auto& GetEntryPointName        () const { return m_entryPointName; }
		const auto& GetShaderModelVersionName() const { return m_shaderModelVersionName; }

	private:

		TypeAlias::ComPtr<IDxcBlob> m_dxcBlob = nullptr;

		JsonConverter::ShaderJsonConverter m_shaderJsonConverter = JsonConverter::ShaderJsonConverter();

		std::filesystem::path m_filePath               = {};
		std::string           m_entryPointName         = {};
		std::string           m_shaderModelVersionName = {};
	};
}