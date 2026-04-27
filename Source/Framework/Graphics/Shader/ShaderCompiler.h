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
		TypeAlias::ComPtr<IDxcBlob> CompileFromFile(const std::wstring& a_filePath, const std::string& a_entryPointName, const std::string& a_shaderModelVersionName) const;

	private:

		static constexpr const wchar_t* const k_argEntryPointOption    = L"-E";
		static constexpr const wchar_t* const k_argTargetProfileOption = L"-T";
		static constexpr const wchar_t* const k_argRowMajorMatrix      = L"-Zpr";
		static constexpr const wchar_t* const k_argHLSLVersionOption   = L"-HV";
		static constexpr const wchar_t* const k_argHLSLVersion         = L"2021";

#if defined(_DEBUG)
		static constexpr const wchar_t* const k_argDebugINFO      = L"-Zi";
		static constexpr const wchar_t* const k_argEmbedDebugINFO = L"-Qembed_debug";
#else
		static constexpr const wchar_t* const k_argOptimizationLevel = L"-O3";
#endif


		TypeAlias::ComPtr<IDxcUtils>     m_dxcUtils    = nullptr;
		TypeAlias::ComPtr<IDxcCompiler3> m_dxcCompiler = nullptr;
	};
}