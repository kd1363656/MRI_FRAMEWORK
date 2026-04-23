#include "ShaderCompiler.h"

bool FWK::Graphics::ShaderCompiler::Create()
{
	// DXC(DX Shader Compiler)関連のCOMオブジェクト作成する関数
	// DxcCreateInstance(何を作るのかを表すCLSID、
	//					 受け取りたいCOMインターフェース型のID、
	//					 作成結果のポインタを書き込むアドレス);

	// 今回はDxcUtilsを作成している。
	// DxcUtilsはファイル読み込みや文字コード変換、include処理補助など、
	// DXCを使う周辺処理で使う便利機能用オブジェクト
	auto l_hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_dxcUtils.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "DxcUtilsの作成に失敗しました。");
		return false;
	}

	// DXC本体のコンパイラオブジェクトを作成する
	// CLSID_DxcCompilerは「シェーダーをコンパイルする本体」を表す識別子。
	// これを作成することで、HLSLコードをDXILへコンパイルできるようになる。
	l_hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_dxcCompiler.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "DxcCompilerの作成に失敗しました。");
		return false;
	}

	return true;
}

FWK::TypeAlias::ComPtr<IDxcBlob> FWK::Graphics::ShaderCompiler::CompileFromFile(const std::filesystem::path& a_filePath, const std::string& a_entryPointName, const std::string& a_shaderModelVersionName) const
{
	TypeAlias::ComPtr<IDxcBlobEncoding> l_source = nullptr;

	if (!m_dxcUtils)
	{
		assert(false && "DxcUtilsの作成がされておらず、シェーダーファイルの読み込みが出来ませんでした。");
		return nullptr;
	}

	if (!m_dxcCompiler)
	{
		assert(false && "DxcCompilerの作成がされておらず、シェーダーファイルの読み込みが出来ませんでした。");
		return nullptr;
	}

	if (a_filePath.empty())
	{
		assert(false && "ファイルパスが空文字列のため読み込めませんでした");
		return nullptr;
	}

	if (a_entryPointName.empty())
	{
		assert(false && "エントリーポイント名が空文字列のため読み込めませんでした");
		return nullptr;
	}

	if (a_shaderModelVersionName.empty())
	{
		assert(false && "シェーダーモデルバージョン名が空文字列のため読み込めませんでした");
		return nullptr;
	}

	// HLSLファイルを読み込む関数
	// LoadFile(読み込むファイルパス、
	//			文字コード指定(nullptrなら自動又は基底処理)、
	//			読み込んだ結果を書き込むIDxcBlobEncodingの受取先);

	// 読み込んだHLSLソースコードはl_sourceに入る。
	auto l_hr = m_dxcUtils->LoadFile(a_filePath.wstring().c_str(), nullptr, l_source.ReleaseAndGetAddressOf());

	if (FAILED(l_hr))
	{
#if defined(_DEBUG)
		OutputDebugStringA("HLSLファイルのロードに失敗しました。");
#endif
		return nullptr;
	}
	
	// DXCへ渡す入力バッファ情報を作る
	// DxcBufferは「このメモリ範囲をHLSLソースコードとして読んでください」
	// とDXCへ伝えるための構造体
	DxcBuffer l_buffer = {};

	// HLSLソースコードの先頭アドレス
	l_buffer.Ptr = l_source->GetBufferPointer();

	// HLSLソースコードのサイズ(バイト単位)
	l_buffer.Size = l_source->GetBufferSize();

	// ソースコードの文字コード
	// 今回はUTF-8として扱う
	l_buffer.Encoding = DXC_CP_UTF8;

	// DXCへ渡すコンパイル引数一覧
	// コマンドラインのdxc.exeに渡すオプションとほぼ同じ意味を持つ
	std::vector<LPCWSTR> l_args = {};

	const auto& l_filePath               = a_filePath.wstring				  ();
	const auto& l_entryPointName         = Utility::String::StringToWideString(a_entryPointName);
	const auto& l_shaderModelVersionName = Utility::String::StringToWideString(a_shaderModelVersionName);

	// 入力ファイル名
	// エラーメッセージ表示などに使われる
	l_args.emplace_back(l_filePath.c_str());
	
	// -E : エントリーポイント名を指定するオプション
	l_args.emplace_back(k_argEntryPointOption);

	// どの関数をコンパイル開始地点にするか
	// 例 : VSMain / PSMain
	l_args.emplace_back(l_entryPointName.c_str());

	// -T : シェーダープロファイル(シェーダーモデル)を指定するオプション
	l_args.emplace_back(k_argTargetProfileOption);

	// どの種類のシェーダーとしてコンパイルするか
	// 例 : vs_6_0 = VertexShader / ps_6_0 = PixelShader
	l_args.emplace_back(l_shaderModelVersionName.c_str());

	// -Zpr : 行優先(Row - major)で行列を扱う指定
	// CPU側とHLSL側の行列の並びを揃えたいときに重要
	l_args.emplace_back(k_argRowMajorMatrix);

	// -HV : HLSL言語バージョン指定オプション
	l_args.emplace_back(k_argHLSLVersionOption);

	// -2021 : HLSL 2021として解釈する
	l_args.emplace_back(k_argHLSLVersion);

#if defined(_DEBUG)

	// Zi : デバッグ情報を付ける
	l_args.emplace_back(k_argDebugINFO);

	// -Qembed_debug : デバッグ情報を出力バイナリ内に埋め込む
	l_args.emplace_back(k_argEmbedDebugINFO);
#else
	// -03 : 最適化を強める(リリース向け)
	l_args.emplace_back(k_argOptimizationLevel);
#endif

	TypeAlias::ComPtr<IDxcResult>         l_result		   = nullptr;
	TypeAlias::ComPtr<IDxcIncludeHandler> l_includeHandler = nullptr;

	// HLSLのインクルードハンドラを作成する関数
	// CreateDefaultIncludeHandler(受け取りたいCOMインターフェース型のID、
	//					           作成結果のポインタを書き込むアドレス);

	// これがないと、シェーダー内で"includeを使った時に参照先を解決できない
	l_hr = m_dxcUtils->CreateDefaultIncludeHandler(l_includeHandler.ReleaseAndGetAddressOf());

	if (FAILED(l_hr))
	{
#if defined(_DEBUG)
		assert(false && "インクルードハンドラの作成に失敗しました。");
#endif
		return nullptr;
	}



	// HLSLをコンパイルする関数
	// Compile(コンパイルするソース情報、
	//		   コンパイル引数配列、
	//		   引数の数、
	//		   #include解決用ハンドラ、
	//		   受け取りたい結果のインターフェース型と格納先);

	l_hr = m_dxcCompiler->Compile(&l_buffer,
								  l_args.data(),
								  static_cast<UINT>(l_args.size()),
								  l_includeHandler.Get(),
								  IID_PPV_ARGS(l_result.GetAddressOf()));

	if (FAILED(l_hr))
	{
#if defined(_DEBUG)
		OutputDebugStringA("DXCコンパイル呼び出しに失敗しました。");
#endif
		return nullptr;
	}

	// DXCの呼び出し自体は成功しても、
	// HLSLコードの文法エラーなどでコンパイル結果が失敗している場合がある
	// そのためGetStatus()で「実際のコンパイル結果」を確認する必要がある
	HRESULT l_compileStatus = {};

	l_result->GetStatus(&l_compileStatus);

	if (FAILED(l_compileStatus))
	{
		TypeAlias::ComPtr<IDxcBlobUtf8> l_errors = nullptr;

		// エラーメッセージ文字列を取得する関数
		// GetOutput(何の出力を取得するか、
		//			 受け取りたいインターフェース型と格納先、
		//			 補助情報の受取先(今回は使わないのでnullptr));

		l_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(l_errors.ReleaseAndGetAddressOf()), nullptr);

		// エラー文字列が存在するならデバッグ出力へ表示する
		if (l_errors && l_errors->GetStringLength() > 0)
		{
			OutputDebugStringA(l_errors->GetStringPointer());
		}

#if defined(_DEBUG)
		OutputDebugStringA("HLSLのコンパイルに失敗しました。");
#endif
		return nullptr;
	}

	TypeAlias::ComPtr<IDxcBlob> l_outBlob = nullptr;

	// コンパイル結果の中から生成されたDXILバイトコードを取得する関数
	// GetOutput(どの種類の出力を取るか、
	//			 受け取りたいCOMインターフェース型のID、
	//			 作成結果のポインタを書き込むアドレス,
	//			 出力名);

	l_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(l_outBlob.ReleaseAndGetAddressOf()), nullptr);

	if (!l_outBlob)
	{
#if defined(_DEBUG)
		OutputDebugStringA("DXILバイトコードの取得に失敗しました。");
#endif
		return nullptr;
	}

	return l_outBlob;
}