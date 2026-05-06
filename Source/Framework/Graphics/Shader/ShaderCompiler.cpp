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

FWK::TypeAlias::ComPtr<IDxcBlob> FWK::Graphics::ShaderCompiler::LoadBinaryFromFile(const std::wstring& a_filePath) const
{
	if (!m_dxcUtils)
	{
		assert(false && "DxcUtilsの作成がされておらず、シェーダーバイナリファイルの読み込みができませんでした。");
		return nullptr;
	}

	if (a_filePath.empty())
	{
		assert(false && "ファイルパスが空文字列のため、シェーダーバイナリファイルを読み込みませんでした。");
		return nullptr;
	}

	TypeAlias::ComPtr<IDxcBlobEncoding> l_binaryBlob = nullptr;

	// LoadFile(読み込むファイルパス、
	//			文字コード指定(nullptrなら自動又はバイナリとして扱う)、
	//			読み込んだ結果を書き込むIDxcBlobEncodingの受取先);
	// .csoはテキストではなくDXILバイトコードなので、
	// ここではコンパイルせず、そのままBlobとして読み込む
	auto l_hr = m_dxcUtils->LoadFile(a_filePath.c_str(), nullptr, l_binaryBlob.ReleaseAndGetAddressOf());

	if (FAILED(l_hr))
	{
#if defined(_DEBUG)
		OutputDebugStringA("シェーダーバイナリファイルのロード処理に失敗しました。");
#endif
		return nullptr;
	}

	TypeAlias::ComPtr<IDxcBlob> l_dxcBlob = nullptr;

	// IDxcBlobEncodingはIDxcBlobを継承しているため、
	// PSO作成で使うD3D12_SHADER_BYTECODE用のIDxcBlobとして扱えるように変換する。
	l_hr = l_binaryBlob.As(&l_dxcBlob);

	if (FAILED(l_hr))
	{
		assert(false && "IDxcBlobEncodingからIDxcBlobへの変換に失敗しました。");
		return nullptr;
	}

	return l_dxcBlob;
}