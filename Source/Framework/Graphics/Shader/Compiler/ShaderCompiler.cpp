#include "ShaderCompiler.h"

void FWK::Graphics::ShaderCompiler::Init()
{
	m_dxcUtils.Reset   ();
	m_dxcCompiler.Reset();
}
bool FWK::Graphics::ShaderCompiler::Create()
{
	// DXCのユーティリティー機能(ファイル読み込みやInclude処理など)を提供するIDxcUtilsを作成
	auto l_hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_dxcUtils.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "DxcUtilsの作成に失敗しました。");
		return false;
	}

	// HLSLをDXILへコンパイルするIDxcCompilerを作成
	l_hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_dxcCompiler.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "DxcCompilerの作成に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Graphics::ShaderCompiler::CompileFromFile(const std::string& a_filePath,
													const std::string& a_entryPointName,
													const std::string& a_shaderModelName,
												    ComPtr<IDxcBlob>&  a_outBlob) const
{
	ComPtr<IDxcBlobEncoding> l_source = nullptr;

	if (!m_dxcUtils)
	{
		assert(false && "DxcUtilsの作成がされておらず、シェーダーファイルの読み込みができませんでした。");
		return false;
	}

	// フィルパス、エントリーポイント名、シェーダーモデル名をワイド文字列型に変換
	const auto& l_filePath        = Utility::String::StringToWString(a_filePath);
	const auto& l_entryPointName  = Utility::String::StringToWString(a_entryPointName);
	const auto& l_shaderModelName = Utility::String::StringToWString(a_shaderModelName);

	// ファイルパス、ファイルの文字コードをどう扱うか(nullptrは自動で文字コードを取り扱う)、読み込んだファイルの中身を格納
	auto l_hr = m_dxcUtils->LoadFile(l_filePath.c_str(), nullptr, l_source.GetAddressOf());

	if (FAILED(l_hr))
	{
#if defined(_DEBUG)
		OutputDebugStringA("HLSLファイルのロードに失敗しました。");
		a_outBlob.Reset();
#endif
		return false;
	}

	// DXCのCompile関数に渡す入力ソース情報(DxcBuffer)を構築
	DxcBuffer l_buffer = {};

	l_buffer.Ptr      = l_source->GetBufferPointer(); // HLSLソースコードの先頭ポインタ
	l_buffer.Size     = l_source->GetBufferSize   (); // サイズ
	l_buffer.Encoding = DXC_CP_UTF8;				  // UTF-8として解釈

	// コンパイル引数設定
	std::vector<LPCWSTR> l_args;
	l_args.emplace_back(l_filePath.c_str());	    // コンパイル時に使用される入力ファイル名(エラー表示などに使われる)
	l_args.emplace_back(L"-E");
	l_args.emplace_back(l_entryPointName.c_str());  // コンパイル開始関数名(例 : VSMain / PSMain)
	l_args.emplace_back(L"-T");
	l_args.emplace_back(l_shaderModelName.c_str()); // ターゲットプロファイル(例 : vs_6_0 / ps_6_0)
	l_args.emplace_back(L"-Zpr");					// ※超重要、行優先(Row - major) 指定(C++側の行列レイアウトと一致させるため重要)
	l_args.emplace_back(L"-HV");					// HLSLの言語バージョン指定
	l_args.emplace_back(L"2021");					// HLSL 2021 言語仕様を使用

#if defined(_DEBUG)
	l_args.emplace_back(L"-Zi");		   // デバック情報を生成
	l_args.emplace_back(L"-Qembed_debug"); // デバック情報をDXILへ埋め込み
#else
	l_args.emplace_back(L"-O3");		   // 最適化レベル最大
#endif
	
	// コンパイル結果格納変数
	ComPtr<IDxcResult> l_result = nullptr;

	// HLSL内部でファイルをインクルードできるようにするための変数
	ComPtr<IDxcIncludeHandler> l_includeHandler = nullptr;

	m_dxcUtils->CreateDefaultIncludeHandler(l_includeHandler.ReleaseAndGetAddressOf());

	if (!l_includeHandler)
	{
#if defined(_DEBUG)
		OutputDebugStringA("インクルードハンドラの作成に失敗しました。");
		a_outBlob.Reset();
#endif
		return false;
	}

	// コンパイル実行
	l_hr = m_dxcCompiler->Compile(&l_buffer,
							      l_args.data                       (),
							      static_cast<UINT>(l_args.size     ()),
							      l_includeHandler.Get              (),
							      IID_PPV_ARGS(l_result.GetAddressOf()));

	if (FAILED(l_hr))
	{
#if defined(_DEBUG)
		OutputDebugStringA("DXCコンパイル呼び出しに失敗しました。");
		a_outBlob.Reset();
#endif
		return false;
	}

	// コンパイル結果のステータス確認
	HRESULT l_compileStatus = {};

	l_result->GetStatus(&l_compileStatus);

	// コンパイル結果がFAILEDなら実行
	if (FAILED(l_compileStatus))
	{
		// エラーメッセージを取得
		ComPtr<IDxcBlobUtf8> l_errors = nullptr;

		l_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(l_errors.ReleaseAndGetAddressOf()), nullptr);
		if (l_errors && l_errors->GetStringLength() > 0)
		{
			OutputDebugStringA(l_errors->GetStringPointer());
		}

#if defined(_DEBUG)
		OutputDebugStringA("HLSLのコンパイルに失敗しました。");
		a_outBlob.Reset();
#endif
		return false;
	}

	// コンパイル済みDXILバイトコード取得
	l_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(a_outBlob.ReleaseAndGetAddressOf()), nullptr);

	if (!a_outBlob)
	{
#if defined(_DEBUG)
		OutputDebugStringA("DXILバイトコードの取得に失敗しました。。");
		a_outBlob.Reset();
#endif
		return false;
	}

	return true;
}