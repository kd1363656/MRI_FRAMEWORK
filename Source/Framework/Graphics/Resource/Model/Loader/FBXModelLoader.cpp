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

	// FbxManagerに読み込み設定を登録する
	// SetIOSettings(使用するFBX読み込み設定)
	m_fbxManager->SetIOSettings(l_fbxIOSettings);

	return true;
}

bool FWK::Graphics::FBXModelLoader::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::StaticModelData& a_staticModelData) const
{
	if (!m_fbxManager)
	{
		assert(false && "FbxManagerが作成されておらず、FBX読み込みに失敗しました。");
		return false;
	}

	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerFBXExtension))
	{
		assert(false && "ファイルパスが読み込めるファイルパスでなく、FBX読み込みに失敗しました。");
		return false;
	}

	// 念のため初期化
	a_staticModelData = {};

	// FBXファイルの内容を一時的に保存するSceneを作成する
	// FbxScene::Create(FbxManager, 
	//					シーン名);

	auto* l_fbxScene = FbxScene::Create(m_fbxManager, k_defaultSceneName.data());

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneの作成に失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// FBXファイルを読み込むImporterを作成する
	// FbxImporter::Create(FbxManager,
	//					   Importer名);

	auto* l_fbxImporter = FbxImporter::Create(m_fbxManager, k_defaultModelImporterName.data());

	if (!l_fbxImporter)
	{
		l_fbxScene->Destroy();

		assert(false && "FBXImporterの作成に失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// FBXファイルを開く
	// Initialize(FBXファイルパス、
	//			  ファイル形式自動判定用のID(-1で自動判定)、
	//			  FbxManagerに登録した読み込み設定);

	if (const auto& l_filePathString = a_filePath.string();
		!l_fbxImporter->Initialize(l_filePathString.c_str(), k_autoDetectFBXFileFormatID, m_fbxManager->GetIOSettings()))
	{
		l_fbxImporter->Destroy();
		l_fbxScene->Destroy   ();

		assert(false && "Importer::Initializeに失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// Importerで開いたFBXファイル内容をSceneへ読み込む
	// Import(読み込み先のFBXシーン);

	if (!l_fbxImporter->Import(l_fbxScene))
	{
		l_fbxImporter->Destroy();
		l_fbxScene->Destroy   ();

		assert(false && "FbxImporter::Importに失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// ImporterはSceneへ読み込み終わったら不要なので破棄する
	l_fbxImporter->Destroy();

	// FBX内のジオメトリを変換するためのコンバーターを作成する
	// ここではメッシュを三角形ポリゴンへ変換するために使用する
	// FbxGeometryConverter(FbxManager);
	
	// Triangulate(FBXシーン、
	//			   変換後の属性を置き換えるかどうか);
	// trueにすることでScene内のジオメトリを三角形化後のものへ置き換える

	if (FbxGeometryConverter l_fbxGeometryConverter(m_fbxManager);
		!l_fbxGeometryConverter.Triangulate(l_fbxScene, true))
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

	// FBXのNode階層を再帰的に探索し、Meshを持つNodeから静的メッシュ情報を抽出する
	// a_fbxNode		 : 探索開始Node
	// a_staticModelData : 抽出したStaticModelMeshの追加先
	if (!ExtractMeshFromNode(l_rootNode, a_staticModelData))
	{
		l_fbxScene->Destroy();

		assert(false && "FBXシーンからメッシュ情報の抽出に失敗しました。");
		return false;
	}

	l_fbxScene->Destroy();

	if (a_staticModelData.m_staticModelMeshList.empty())
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

bool FWK::Graphics::FBXModelLoader::ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::StaticModelData& a_staticModelData) const
{
	if (!a_fbxNode)
	{
		assert(false && "FbxNodeが無効のため、FBXメッシュ抽出に失敗しました。");
		return false;
	}
	
	// Nodeがっ持つ属性を取得する
	// 属性にはMesh / Camera / Light / Skeltonなどがある
	// Nodeの属性がMeshの場合のみFbxMeshとして取得する
	if (const auto* l_fbxNodeAttribute = a_fbxNode->GetNodeAttribute();
		l_fbxNodeAttribute					   &&
		l_fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		auto* l_fbxMesh = a_fbxNode->GetMesh();

		if (!l_fbxMesh)
		{
			assert(false && "FbxNodeからFbxMeshの取得に失敗しました。");
			return false;
		}

		Struct::StaticModelMesh l_staticModelMesh = {};

		// FbxMeshからStaticModelMeshへ頂点情報とインデックス情報を変換する
		// 現在は三角形一つにつき頂点3つを作成する
		// 頂点の重複削除や最適化はまだ行わない
		if (!ExtractMesh(l_fbxMesh, l_staticModelMesh))
		{
			assert(false && "FbxMeshからStaticModelMeshへの変換に失敗しました。");
			return false;
		}

		if (!l_staticModelMesh.m_staticModelVertexList.empty() &&
			!l_staticModelMesh.m_indexList.empty())
		{ 
			a_staticModelData.m_staticModelMeshList.emplace_back(std::move(l_staticModelMesh));
		}
	}

	// FBXはNode階層でデータを持っているため、子Nodemも再帰的に探索する
	const auto l_childCount = a_fbxNode->GetChildCount();

	for (int l_childIndex = 0; l_childIndex < l_childCount; ++l_childIndex)
	{
		if (!ExtractMeshFromNode(a_fbxNode->GetChild(l_childIndex), a_staticModelData))
		{
			return false;
		}
	}

	return true;
}

bool FWK::Graphics::FBXModelLoader::ExtractMesh(FbxMesh* a_fbxMesh, Struct::StaticModelMesh& a_staticModelMesh) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、StaticModelMeshへの変換に失敗しました。");
		return false;
	}

	a_staticModelMesh = {};

	// FbxMeshに含まれるポリゴン数を取得する
	// Triangulate済みなので、基本的に1ポリゴンは1三角形になる
	const auto l_polygonCount = a_fbxMesh->GetPolygonCount();

	if (l_polygonCount <= k_emptyPolygonCount) { return true; }

	// FBXメッシュが持っているUVセット名一覧を取得する
	// FBXは複数のUVセットを持てるが、現在は最初のUVセットだけを使用する
	FbxStringList l_uvSetNameList = {};

	// GetUVSetNames(UVセット名一覧の格納先);
	a_fbxMesh->GetUVSetNames(l_uvSetNameList);

	const char* l_uvSetName = nullptr;

	if (l_uvSetNameList.GetCount() > k_emptyUVSetCount)
	{
		l_uvSetName = l_uvSetNameList.GetStringAt(k_firstUVSetIndex);
	}

	a_staticModelMesh.m_staticModelVertexList.reserve(static_cast<std::size_t>(l_polygonCount) * k_triangleVertexCount);
	a_staticModelMesh.m_indexList.reserve			 (static_cast<std::size_t>(l_polygonCount) * k_triangleVertexCount);

	for (int l_polygonIndex = 0; l_polygonIndex < l_polygonCount; ++l_polygonIndex)
	{
		// 現在のポリゴンを構成する頂点数を取得する
		// Triangulate済みなので3頂点であることを期待する
		if (const auto l_polygonVertexCount = a_fbxMesh->GetPolygonSize(l_polygonIndex);
			l_polygonVertexCount != k_triangleVertexCount)
		{
			assert(false && "三角形化後のFBXメッシュに三角形以外のポリゴンが含まれています。");
			return false;
		}

		for (int l_polygonVertexIndex = 0; l_polygonVertexIndex < k_triangleVertexCount; ++l_polygonVertexIndex)
		{
			if (a_staticModelMesh.m_staticModelVertexList.size() >= std::numeric_limits<std::uint32_t>::max())
			{
				assert(false && "StaticModelMeshの頂点数がuint32_tで扱える範囲を超えたため、FBX読み込みに失敗しました。");
				return false;
			}

			// ポリゴン頂点が参照しているControlPointの番号を取得する
			// GetPolygonVertex(ポリゴン番号、
			//					ポリゴン内の頂点番号);

			const auto l_controlPointIndex = a_fbxMesh->GetPolygonVertex(l_polygonIndex, l_polygonVertexIndex);

			Struct::StaticModelVertex l_staticModelVertex = {};

			// ControlPointIndexから頂点座標を取得する
			// FBXでは頂点座標はControlPointとして保持される
			l_staticModelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_controlPointIndex);

			// ポリゴン頂点に対応する法線を取得する
			// FBXでは法線がControlPoint単位ではなく、ポリゴン頂点単位で異なる場合がある
			l_staticModelVertex.m_normal = FetchVertexNormal(a_fbxMesh, l_polygonIndex, l_polygonVertexIndex);

			// ポリゴン頂点に対応するUVを取得する
			// FBXではUVがポリゴン頂点単位で異なる場合がある
			// 読み込んだV座標はエンジン側のUVに合わせるため反転する
			l_staticModelVertex.m_uv = FetchVertexUV(a_fbxMesh,
													 l_polygonIndex,
													 l_polygonVertexIndex,
													 l_uvSetName);

			const auto l_index = static_cast<std::uint32_t>(a_staticModelMesh.m_staticModelVertexList.size());

			a_staticModelMesh.m_staticModelVertexList.emplace_back(l_staticModelVertex);
			a_staticModelMesh.m_indexList.emplace_back			  (l_index);
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

	if (a_controlPointIndex < k_minControlPointIndex ||
		a_controlPointIndex >= a_fbxMesh->GetControlPointsCount())
	{
		assert(false && "ControlPointIndexが範囲外のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	const auto* l_controlPointList = a_fbxMesh->GetControlPoints();

	if (!l_controlPointList)
	{
		assert(false && "FBXメッシュのControlPointListが無効のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	const auto& l_controlPoint = l_controlPointList[a_controlPointIndex];

	return ConvertFbxVector4ToVector3(l_controlPoint);
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

	return ConvertFbxVector4ToVector3(l_fbxNormal);
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

	// GetPolygonVertexUV(ポリゴン番号、
	//					  ポリゴン内の頂点番号、
	//					  使用するUVセット名、
	//					  取得したUVの格納先、
	//					  UVが未割り当てかどうかの格納先);
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
		static_cast<float>(l_fbxUV[k_fbxUVUIndex]),
		k_uvVFlipOffset - static_cast<float>(l_fbxUV[k_fbxUVVIndex])
	};
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoader::ConvertFbxVector4ToVector3(const FbxVector4& a_fbxVector) const 
{
	return TypeAlias::Math::Vector3
	{
		static_cast<float>(a_fbxVector[k_fbxVectorXIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorYIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorZIndex])
	};
}