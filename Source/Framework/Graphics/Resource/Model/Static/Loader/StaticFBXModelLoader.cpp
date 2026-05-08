#include "StaticFBXModelLoader.h"

bool FWK::Graphics::StaticFBXModelLoader::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const
{
	auto* l_fbxManager = GetPTRFBXManager();

	if (!l_fbxManager)
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
	a_modelData = {};

	// FBXファイルの内容を一時的に保存するSceneを作成する
	// FbxScene::Create(FbxManager, 
	//					シーン名);

	auto* l_fbxScene = FbxScene::Create(l_fbxManager, Constant::k_defaultFBXSceneName.data());

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneの作成に失敗したため、FBX読み込みに失敗しました。");
		return false;
	}

	// FBXファイルを読み込むImporterを作成する
	// FbxImporter::Create(FbxManager,
	//					   Importer名);

	auto* l_fbxImporter = FbxImporter::Create(l_fbxManager, Constant::k_defaultFBXModelImporterName.data());

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
		!l_fbxImporter->Initialize(l_filePathString.c_str(), Constant::k_autoDetectFBXFileFormatID, l_fbxManager->GetIOSettings()))
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

	if (FbxGeometryConverter l_fbxGeometryConverter(l_fbxManager);
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
	// a_fbxNode   : 探索開始Node
	// a_modelData : 抽出したModelMeshの追加先
	if (!ExtractMeshFromNode(l_rootNode, a_modelData))
	{
		l_fbxScene->Destroy();

		assert(false && "FBXシーンからメッシュ情報の抽出に失敗しました。");
		return false;
	}

	l_fbxScene->Destroy();

	if (a_modelData.m_modelMeshList.empty())
	{
		assert(false && "FBX内に読み込み可能なメッシュが存在しません。");
		return false;
	}

	return true;
}

bool FWK::Graphics::StaticFBXModelLoader::ExtractMeshFromNode(FbxNode* a_fbxNode, Struct::ModelData& a_modelData) const
{
	if (!a_fbxNode)
	{
		assert(false && "FbxNodeが無効のため、FBXメッシュ抽出に失敗しました。");
		return false;
	}
	
	// Nodeが持つ属性を取得する
	// 属性にはMesh / Camera / Light / Skeletonなどがある
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

		Struct::ModelMesh l_modelMesh = {};

		// FbxMeshからModelMeshへ頂点情報とインデックス情報を変換する
		// 現在は三角形一つにつき頂点3つを作成する
		// 頂点の重複削除や最適化はまだ行わない
		if (!ExtractMesh(l_fbxMesh, l_modelMesh))
		{
			assert(false && "FbxMeshからModelMeshへの変換に失敗しました。");
			return false;
		}

		if (!l_modelMesh.m_modelVertexList.empty() &&
			!l_modelMesh.m_indexList.empty())
		{ 
			a_modelData.m_modelMeshList.emplace_back(std::move(l_modelMesh));
		}
	}

	// FBXはNode階層でデータを持っているため、子Nodeも再帰的に探索する
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

bool FWK::Graphics::StaticFBXModelLoader::ExtractMesh(FbxMesh* a_fbxMesh, Struct::ModelMesh& a_modelMesh) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、ModelMeshへの変換に失敗しました。");
		return false;
	}

	a_modelMesh = {};

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

	a_modelMesh.m_modelVertexList.reserve(static_cast<std::size_t>(l_polygonCount) * Constant::k_triangleVertexCount);
	a_modelMesh.m_indexList.reserve	     (static_cast<std::size_t>(l_polygonCount) * Constant::k_triangleVertexCount);

	for (int l_polygonIndex = 0; l_polygonIndex < l_polygonCount; ++l_polygonIndex)
	{
		// 現在のポリゴンを構成する頂点数を取得する
		// Triangulate済みなので3頂点であることを期待する
		if (const auto l_polygonVertexCount = a_fbxMesh->GetPolygonSize(l_polygonIndex);
			l_polygonVertexCount != Constant::k_triangleVertexCount)
		{
			assert(false && "三角形化後のFBXメッシュに三角形以外のポリゴンが含まれています。");
			return false;
		}

		for (int l_polygonVertexIndex = 0; l_polygonVertexIndex < Constant::k_triangleVertexCount; ++l_polygonVertexIndex)
		{
			if (a_modelMesh.m_modelVertexList.size() >= std::numeric_limits<std::uint32_t>::max())
			{
				assert(false && "ModelMeshの頂点数がuint32_tで扱える範囲を超えたため、FBX読み込みに失敗しました。");
				return false;
			}

			// ポリゴン頂点が参照しているControlPointの番号を取得する
			// GetPolygonVertex(ポリゴン番号、
			//					ポリゴン内の頂点番号);

			const auto l_controlPointIndex = a_fbxMesh->GetPolygonVertex(l_polygonIndex, l_polygonVertexIndex);

			Struct::ModelVertex l_modelVertex = {};

			// ControlPointIndexから頂点座標を取得する
			// FBXでは頂点座標はControlPointとして保持される
			l_modelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_controlPointIndex);

			// ポリゴン頂点に対応する法線を取得する
			// FBXでは法線がControlPoint単位ではなく、ポリゴン頂点単位で異なる場合がある
			l_modelVertex.m_normal = FetchVertexNormal(a_fbxMesh, l_polygonIndex, l_polygonVertexIndex);

			// ポリゴン頂点に対応するUVを取得する
			// FBXではUVがポリゴン頂点単位で異なる場合がある
			// 読み込んだV座標はエンジン側のUVに合わせるため反転する
			l_modelVertex.m_uv = FetchVertexUV(a_fbxMesh,
											   l_polygonIndex,
											   l_polygonVertexIndex,
											   l_uvSetName);

			const auto l_index = static_cast<std::uint32_t>(a_modelMesh.m_modelVertexList.size());

			a_modelMesh.m_modelVertexList.emplace_back(l_modelVertex);
			a_modelMesh.m_indexList.emplace_back	  (l_index);
		}
	}

	return true;
}