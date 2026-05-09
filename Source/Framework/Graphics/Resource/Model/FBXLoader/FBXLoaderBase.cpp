#include "FBXLoaderBase.h"

FbxScene* FWK::Graphics::FBXLoaderBase::ImportScene(const std::filesystem::path& a_filePath, FbxManager* a_fbxManager) const
{
	if (!a_fbxManager)
	{
		assert(false && "FbxManagerが作成されていないため、FBXファイルの読み込みに失敗しました。");
		return nullptr;
	}

	// 読み込み可能なファイルパスかどうかの確認を行う
	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerFBXExtension)) { return nullptr; }

	// std::filesystem::pathをFBXSDKに渡せる文字列へ変換する
	const auto& l_filePathString = a_filePath.string();

	// FbxSceneについて
	// FBXファイルから読み込んだノード、メッシュ、マテリアル、アニメーションなどを保持する入れ物
	// FbxScene::Create(FBXSDKオブジェクトを管理するFbxManager、
	//					作成するFbxSceneの識別名);

	auto* l_fbxScene = FbxScene::Create(a_fbxManager, k_defaultSceneName.data());

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneの作成に失敗しました。");
		return nullptr;
	}

	// FbxImporter::Create(FBXSDKオブジェクトを管理するFbxManager、
	//					   作成するFbxImporterの識別名);

	auto* l_fbxImporter = FbxImporter::Create(a_fbxManager, k_defaultImporterName.data());

	if (!l_fbxImporter)
	{
		assert(false && "FbxImporterの作成に失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return nullptr;
	}

	// FbxImporter::Initialize(読み込むFBXファイルパス、
	//						   使用するファイル形式ID、
	//						   FbxManagerに登録されている入出力設定);

	if (!l_fbxImporter->Initialize(l_filePathString.c_str(), k_autoDetectFileFormat, a_fbxManager->GetIOSettings()))
	{
		assert(false && "FbxImporterの初期化に失敗しました。");

		l_fbxImporter->Destroy();
		l_fbxScene->Destroy   ();

		l_fbxImporter = nullptr;
		l_fbxScene	  = nullptr;

		return nullptr;
	}

	// FbxImporter::Import(FBXファイルの読み込み結果を格納するFbxScene);

	const bool l_isImported = l_fbxImporter->Import(l_fbxScene);

	// FbxImporterはFBXファイルをFbxSceneへ読み込むための一時オブジェクトなので、
	// Import完了後は成功・失敗に関係なく破棄する
	l_fbxImporter->Destroy();
	l_fbxImporter = nullptr;

	if (!l_isImported)
	{
		assert(false && "FBXファイルのImportに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return nullptr;
	}

	return l_fbxScene;
}