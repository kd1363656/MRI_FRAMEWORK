#pragma once

namespace FWK::Graphics
{
	class ShaderCompiler final
	{
	public:

		ShaderCompiler () = default;
		~ShaderCompiler() = default;

		void Init  ();
		bool Create();

		// コンパイル対象のシェーダーを格納するIDxcBlobを引数にしているのは
		// しっかりコンパイルできたかどうかを"bool"で確認するため
		// HLSLファイルを読み込み、指定エントリーポイントとシェーダーモデルでコンパイルし、
		// DXILバイトコードを出力Blobとして返す
		bool CompileFromFile(const std::string&	a_filePath,
							 const std::string& a_entryPointName,
							 const std::string& a_shaderModelName,
							 ComPtr<IDxcBlob>&  a_outBlob) const;

	private:

		ComPtr<IDxcUtils>     m_dxcUtils    = nullptr;
		ComPtr<IDxcCompiler3> m_dxcCompiler = nullptr;
	};
}