#include "FBXLoaderContext.h"

FWK::Graphics::FBXLoaderContext::FBXLoaderContext () = default;
FWK::Graphics::FBXLoaderContext::~FBXLoaderContext()
{
	Destroy();
}

bool FWK::Graphics::FBXLoaderContext::Create()
{
	// FbxManagerが作成されているなら処理を行わない
	if (m_fbxManager)
	{
		assert(false && "FbxManagerは既に作成されています。");
		return false;
	}

	// FBXSDK全体を管理するFbxManagerを作成する
	// FbxManager::Create();

	m_fbxManager = FbxManager::Create();

	if (!m_fbxManager)
	{
		assert(false && "FbxManagerの作成に失敗しました。");
		return false;
	}

	// FBXファイルを読み込み、書き出しするときに使用する設定オブジェクト
	// FbxIOSettings::Create(FBXSDKオブジェクトを管理するFbxManager,
	//						 入出力設定のルート名);

	m_fbxIOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);

	if (!m_fbxIOSettings)
	{
		assert (false && "FbxIOSettingsの作成に失敗しました。");
		Destroy();
		return false;
	}

	// FbxManager::SetIOSettings(FBXSDKの読み書きで使用する入出力設定)

	m_fbxManager->SetIOSettings(m_fbxIOSettings);

	return true;
}

bool FWK::Graphics::FBXLoaderContext::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const
{
	if (!m_fbxManager)
	{
		assert(false && "FbxManagerが作成されていないため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	return m_staticModelFBXLoader.LoadStaticModelFile(a_filePath, a_modelData, m_fbxManager);
}

void FWK::Graphics::FBXLoaderContext::Destroy()
{
	if (!m_fbxManager) { return; }

	// FbxManagerとFbxManagerに所属しているFBXSDKオブジェクトを破棄する
	// FbxManager::Destroy();

	m_fbxManager->Destroy();

	m_fbxManager    = nullptr;
	m_fbxIOSettings = nullptr;
}