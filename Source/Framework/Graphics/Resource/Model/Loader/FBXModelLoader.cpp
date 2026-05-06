#include "FBXModelLoader.h"

FWK::Graphics::FBXModelLoader::FBXModelLoader() = default;
FWK::Graphics::FBXModelLoader::~FBXModelLoader()
{
	Destroy();
}

bool FWK::Graphics::FBXModelLoader::Create()
{
	if (m_fbxManager) { return true; }

	// FBXSDK全体の管理オブジェクトを作成する
	// FbxManager::Create();
	m_fbxManager = FbxManager::Create();

	if (!m_fbxManager)
	{
		assert(false && "FbxManagerの作成処理に失敗したため、FBXModelLoaderの作成に失敗しました。");
		return false;
	}

	// FBXSDKの読み込み設定を作成する
	// FbxIOSettings::Create(FbxManager,
	//						 設定名);
	auto* l_fbxIOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);

	if (!l_fbxIOSettings)
	{
		Destroy();

		assert(false && "FbxIOSettingsの作成に失敗したため、FBXModelLoaderの作成に失敗しました。");
		return false;
	}

	// FbxManagerに組み込み設定を登録する
	// SetIOSettings(使用するFBX読み込み設定)
	m_fbxManager->SetIOSettings(l_fbxIOSettings);

	return true;
}

bool FWK::Graphics::FBXModelLoader::LoadModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const
{
	if (!m_fbxManager)
	{
		assert(false && "FBXManagerが作成されておらず、FBX読み込みに失敗しました。");
		return false;
	}

	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerFBXExtension))
	{
		assert(false && "ファイルパスが読み込めるファイルパスでなく、FBX読み込みに失敗しました。");
		return false;
	}

	// 念のため初期化
	a_modelData = {};

	// FBXシーンをsカウ生する
	// FbxScene::Create(FbxManager, 
	//					シーン名);

	auto* l_fbxScene = FbxScene::Create(m_fbxManager, k_defaultSceneName.data());

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneの作成に失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// FBXファイルを読み込むImporterを作成する
	auto* l_fbxImporter = FbxImporter::Create(m_fbxManager, k_defaultModelImporterName.data());

	if (!l_fbxImporter)
	{
		l_fbxScene->Destroy();

		assert(false && "FBXImporterの作成に失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	const auto l_filePathString = a_filePath.string();

	// FBXファイルを開く
	// Initialize(FBXファイルパス、
	//			  ファイル形式自動判定用のID(-1で自動判定)、
	//			  FbxManagerに登録した読み込み設定);
	if (!l_fbxImporter->Initialize(l_filePathString.c_str(), -1, m_fbxManager->GetIOSettings()))
	{
		l_fbxImporter->Destroy();
		l_fbxScene->Destroy   ();

		assert(false && "Importer::Initializeに失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// Importerで開いたFBXファイル内容をSceneへ読み込む
	// Importer(読み込み先のFBXシーン);
	if (!l_fbxImporter->Import(l_fbxScene))
	{
		l_fbxImporter->Destroy();
		l_fbxScene->Destroy   ();

		assert(false && "FbxImporter::Importに失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// ImporterはSceneへ読み込み終わったら不要なので破棄する
	l_fbxImporter->Destroy();

	// FBX内のメッシュ、NURBS、Patchなどを三角形メッシュへ変換する
	// FbxGeometryConverter(FbxManager);
	FbxGeometryConverter l_fbxGeometryConverter(m_fbxManager);

	// Triangulate(FBXシーンｍｍ
	//			   返還後の属性を置き換えるかどうか);
	// trueにすることでScene内のジオメトリを三角形後のものへ置き換える
	if (!l_fbxGeometryConverter.Triangulate(l_fbxScene, true))
	{
		l_fbxScene->Destroy();

		assert(false && "FbxGeometryConverter::Triangulateに失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// FBXシーンのRootNodeを取得する
	auto* l_rootNode = l_fbxScene->GetRootNode();

	if (!l_rootNode)
	{
		l_fbxScene->Destroy();

		assert(false && "FBXシーンのRootNodeが無効のため、FBX読み込みに失敗しました。");
		return false;
	}

	if (!ExtractMeshFromNode(l_rootNode, a_modelData))
	{
		l_fbxScene->Destroy();

		assert(false && "FBXシーンからメッシュ情報の抽出に失敗しました。");
		return false;
	}

	l_fbxScene->Destroy();

	if (a_modelData.m_meshList.empty())
	{
		assert(false && "FBX内に読み込み可能なメッシュが存在しません。");
		return false;
	}

	return true;
}

void FWK::Graphics::FBXModelLoader::Destroy()
{
	if (!m_fbxManager) { return; }

	m_fbxManager->Destroy();

	m_fbxManager = nullptr;
}

bool FWK::Graphics::FBXModelLoader::ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::ModelData& a_modelData) const
{
	if (!a_fbxNode)
	{
		assert(false && "FbxNodeが無効のため、FBXメッシュ抽出に失敗しました。");
		return false;
	}

	auto* l_fbxNodeAttribute = a_fbxNode->GetNodeAttribute();

	if (l_fbxNodeAttribute && 
		l_fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		auto* l_fbxMesh = a_fbxNode->GetMesh();

		if (!l_fbxMesh)
		{
			assert(false && "FbxNodeからFbxNeshの取得に失敗しました。");
			return false;
		}

		Struct::ModelMesh l_modelMesh = {};

		if (!ExtractMesh(l_fbxMesh, l_modelMesh))
		{
			assert(false && "FbxMeshからModelMeshへの変換に失敗しました。");
			return false;
		}

		if (!l_modelMesh.m_vertexList.empty() &&
			!l_modelMesh.m_indexList.empty())
		{ 
			a_modelData.m_meshList.emplace_back(std::move(l_modelMesh));
		}
	}

	const auto l_childCount = a_fbxNode->GetChildCount();

	for (int l_childIndex = 0; l_childIndex < l_childCount; ++l_childIndex)
	{
		if (!ExtractMeshFromNode(a_fbxNode->GetChild(l_childIndex), a_modelData))
		{
			return false;
		}
	}

	return true;
}

bool FWK::Graphics::FBXModelLoader::ExtractMesh(FbxMesh* a_fbxMesh, Struct::ModelMesh& a_modelMesh) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、ModelMeshへの変換に失敗しました。");
		return false;
	}

	a_modelMesh = {};

	const auto l_polygonCount = a_fbxMesh->GetPolygonCount();

	if (l_polygonCount <= 0) { return true; }

	FbxStringList l_uvSetNameList = {};

	// GetUVSetNames(FBXメッシュが持っているUVセット名一覧の格納先);
	a_fbxMesh->GetUVSetNames(l_uvSetNameList);

	const char* l_uvSetName = nullptr;

	if (l_uvSetNameList.GetCount() > 0)
	{
		l_uvSetName = l_uvSetNameList.GetStringAt(0);
	}

	a_modelMesh.m_vertexList.reserve(static_cast<std::size_t>(l_polygonCount) * k_triangleVertexCount);
	a_modelMesh.m_indexList.reserve (static_cast<std::size_t>(l_polygonCount) * k_triangleVertexCount);

	for (int l_polygonIndex = 0; l_polygonIndex < l_polygonCount; ++l_polygonIndex)
	{
		const auto l_polygonVertexCount = a_fbxMesh->GetPolygonSize(l_polygonIndex);

		if (l_polygonVertexCount != k_triangleVertexCount)
		{
			assert(false && "三角形化後のFBXメッシュに三角形以外のポリゴンが含まれています。");
			return false;
		}

		for (int l_polygonVertexIndex = 0; l_polygonVertexIndex < k_triangleVertexCount; ++l_polygonVertexIndex)
		{
			if (a_modelMesh.m_vertexList.size() >= std::numeric_limits<std::uint32_t>::max())
			{
				assert(false && "ModelMeshの頂点数がuint32_tで扱える範囲を超えたため、FBX読み込みに失敗しました。");
				return false;
			}

			// 戻り値としてControlPointの番号を取得する
			// GetPolygonVertex(ポリゴン番号、
			//					ポリゴン内の頂点番号);

			const auto l_controlPointIndex = a_fbxMesh->GetPolygonVertex(l_polygonIndex, l_polygonVertexIndex);

			Struct::ModelVertex l_modelVertex = {};

			l_modelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_controlPointIndex);

			l_modelVertex.m_normal = FetchVertexNormal(a_fbxMesh, l_polygonIndex, l_polygonVertexIndex);

			l_modelVertex.m_uv = FetchVertexUV(a_fbxMesh, 
											   l_polygonIndex,
											   l_polygonVertexIndex,
											   l_uvSetName);

			const auto l_index = static_cast<std::uint32_t>(a_modelMesh.m_vertexList.size());

			a_modelMesh.m_vertexList.emplace_back(l_modelVertex);
			a_modelMesh.m_indexList.emplace_back (l_index);
		}
	}

	return true;
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoader::FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	if (a_controlPointIndex < 0 ||
		a_controlPointIndex >= a_fbxMesh->GetControlPointsCount())
	{
		assert(false && "ControlPointIndexが範囲外のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	const auto* l_controlPointList = a_fbxMesh->GetControlPoints();

	if (!l_controlPointList)
	{
		assert(false && "FVXメッシュのControlPointListが無効のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	const auto& l_controlPoint = l_controlPointList[a_controlPointIndex];

	return TypeAlias::Math::Vector3
	{
		static_cast<float>(l_controlPoint[0]),
		static_cast<float>(l_controlPoint[1]),
		static_cast<float>(l_controlPoint[2])
	};
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoader::FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、法線の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	FbxVector4 l_fbxNormal = {};

	// GetPolygonVertexNormal(ポリゴン番号、
	//						  ポリゴン内の頂点番号、
	//						  取得した法線の格納先);

	if (!a_fbxMesh->GetPolygonVertexNormal(a_polygonIndex, a_polygonVertexIndex, l_fbxNormal)) { return TypeAlias::Math::Vector3::Zero; }

	return TypeAlias::Math::Vector3
	{
		static_cast<float>(l_fbxNormal[0]),
		static_cast<float>(l_fbxNormal[1]),
		static_cast<float>(l_fbxNormal[2])
	};
}

FWK::TypeAlias::Math::Vector2 FWK::Graphics::FBXModelLoader::FetchVertexUV(const FbxMesh* a_fbxMesh,
																		   const int	  a_polygonIndex,
																		   const int	  a_polygonVertexIndex,
																		   const char*	  a_uvSetName) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、UVの取得に失敗しました。");
		return TypeAlias::Math::Vector2::Zero;
	}

	if (!a_uvSetName) { return TypeAlias::Math::Vector2::Zero; }

	FbxVector2 l_fbxUV = {};

	bool l_isUnmapped = false;

	// GetPolygonVertexUv(ポリゴン番号、
	//					  ポリゴン内の頂点番号、
	//					  使用するUVセット名、
	//					  取得したUVの格納先、
	//					  UVが未割り当かどうかの格納先);
	if (!a_fbxMesh->GetPolygonVertexUV(a_polygonIndex,
									   a_polygonVertexIndex,
									   a_uvSetName,
									   l_fbxUV,
									   l_isUnmapped))
	{
		return TypeAlias::Math::Vector2::Zero;
	}

	if (l_isUnmapped) { return TypeAlias::Math::Vector2::Zero; }

	return TypeAlias::Math::Vector2
	{
		static_cast<float>(l_fbxUV[0]),
		static_cast<float>(l_fbxUV[1] + 1.0F)
	};
}