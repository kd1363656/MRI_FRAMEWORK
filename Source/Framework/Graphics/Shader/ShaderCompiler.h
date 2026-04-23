#pragma once

namespace FWK::Graphics
{
	class ShaderCompiler
	{
	public:

		 ShaderCompiler() = default;
		~ShaderCompiler() = default;

		bool Create();

		// コンパイル対象のシェーダーを格納するIDxcBlobを引数にしているのは
		// しっかりコンパイルできたかどうかを"bool"で確認するため
		// HLSLファイルを読み込み、指定エントリーポイントとシェーダーモデルでコンパイルし、
		// DXILバイトコードを出力Blobとして返す
		TypeAlias::ComPtr<IDxcBlob> CompileFromFile(const std::filesystem::path& a_filePath, const std::string& a_entryPointName, const std::string& a_shaderModelVersionName) const;

	private:

		TypeAlias::ComPtr<IDxcUtils>     m_dxcUtils    = nullptr;
		TypeAlias::ComPtr<IDxcCompiler3> m_dxcCompiler = nullptr;
	};
}