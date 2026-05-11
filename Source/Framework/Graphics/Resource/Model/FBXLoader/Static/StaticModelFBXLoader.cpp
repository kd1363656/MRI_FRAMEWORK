#include "StaticModelFBXLoader.h"

bool FWK::Graphics::StaticModelFBXLoader::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData, FbxManager* a_fbxManager) const
{
	// FBXファイルを読み込み、FbxSceneとして取得する
	// FbxSceneはFBXファイル全体のデータを持つ入れ物
	auto* l_fbxScene = ImportScene(a_filePath, a_fbxManager);

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneへのImportに失敗したため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	// FBX内のMeshは四角形Polygonなどで構成されている場合がある
	// DirectX12で扱うIndexBufferは基本的に三角形単位で描画するため、ここで三角形化しておく
	// FbxGeometryConverter::Triangulate(三角形化するFbxScene、
	//									 元のMeshを三角形化後のMeshに置き換えるかどうか);

	if (FbxGeometryConverter l_geometryConverter(a_fbxManager);
		!l_geometryConverter.Triangulate(l_fbxScene, k_isReplaceOriginalMesh))
	{
		assert(false && "FBXのMeshを三角形化できなかったため、StaticModelの読み込みに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}


	// FBXはNodeの階層構造でデータを持っている
	// RootNodeはその階層構造の一番上のNode
	// FbxScene::GetRootNode();
	auto* l_rootNode = l_fbxScene->GetRootNode();

	if (!l_rootNode)
	{
		assert(false && "RootNodeが存在しないため、StaticModelの読み込みに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	// 出力先のStaticModelDataを初期化する
	// 同じ変数を使いまわした場合に、前回のMeshが残らないようにする
	a_modelData.m_modelMeshList.clear();

	// RootNodeから再帰的に子Nodeを辿り、Meshを持っているNodeを探す
	if (!RecursiveExtractModelMesh(a_modelData, l_rootNode))
	{
		assert(false && "StaticModelとして使用できるMeshNodeが存在しないため、StaticModelの読み込みに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	// Meshが一つも取れなかった場合、このFBXはStaticModelとして扱えない
	if (a_modelData.m_modelMeshList.size() <= k_emptyMeshCount)
	{
		assert(false && "StaticModelとして使用できるModelMeshが存在しないため、StaticModelの読み込みに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	// ImportSceneで作成したFbxSceneを破棄する
	// StaticModelDataには必要な頂点/Indexをコピー済みなので、FbxSceneはここで消してよい
	l_fbxScene->Destroy();
	l_fbxScene = nullptr;

	return true;
}
bool FWK::Graphics::StaticModelFBXLoader::RecursiveExtractModelMesh(Struct::ModelData& a_modelData, FbxNode* a_fbxNode) const
{
	if (!a_fbxNode)
	{
		assert(false && "FbxNodeが無効のため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	// FbxNodeはMesh / Camera / Light / Nullなど、色々な属性を持つ
	// NodeAttributeが存在する場合だけ、属性の種類を確認する
	// NodeAttributeがないNodeもあるが、それは失敗ではない
	// FbxNodeAttribute::GetAttributeType();

	if (const auto* l_fbxNodeAttribute = a_fbxNode->GetNodeAttribute();
		l_fbxNodeAttribute                     &&
		l_fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		// このNodeがMesh属性を持っている場合、FbxMeshを取得する
		// FbxNodeが持っているFbxMeshを取得する
		// FbxNode::GetMesh();

		const auto* l_fbxMesh = a_fbxNode->GetMesh();

		if (!l_fbxMesh)
		{
			assert(false && "FbxMeshが存在しないため、ModelMeshの抽出に失敗しました。");
			return false;
		}

		Struct::ModelMesh l_modelMesh = {};

		// FbxMeshから、自作エンジン用のModelMeshへ変換する
		if (!ExtractModelMesh(l_modelMesh, l_fbxMesh))
		{
			assert(false && "FbxMeshからModelMeshの作成に失敗しました。");
			return false;
		}

		a_modelData.m_modelMeshList.emplace_back(l_modelMesh);
	}

	// 現在のNodeの子Node数を取得する
	// FbxNode::GetChildCount();

	const int l_childNodeCount = a_fbxNode->GetChildCount();

	for (int l_childNodeIndex = 0; l_childNodeIndex < l_childNodeCount; ++l_childNodeIndex)
	{
		// 指定Indexの子Nodeを取得する
		// FbxNode::GetChild(取得したい子ノードのIndex);

		auto* l_childNode = a_fbxNode->GetChild(l_childNodeIndex);

		// 子Nodeも同じようにMeshを持っているか調べる
		if (!RecursiveExtractModelMesh(a_modelData, l_childNode))
		{
			assert(false && "子FbxNodeからModelMeshの抽出に失敗しました。");
			return false;
		}
	}

	return true;
}
bool FWK::Graphics::StaticModelFBXLoader::ExtractModelMesh(Struct::ModelMesh& a_modelMesh, const FbxMesh* a_fbxMesh) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、ModelMeshの作成に失敗しました。");
		return false;
	}

	FbxStringList l_uvSetNameList = {};

	// FBXのMeshは複数のUVSetを持つことができる
	// 例 : 通常UV、ライトマップUVなど
	// 今は最初のUVSetだけ使う
	// FbxMesh::GetUVSetNames(UVSet名の格納先);

	a_fbxMesh->GetUVSetNames(l_uvSetNameList);

	const char* l_uvSetName = nullptr;

	// 現在は一つのUVSetのみ使用
	if (l_uvSetNameList.GetCount() > k_emptyUVSetNameCount)
	{
		l_uvSetName = l_uvSetNameList.GetStringAt(k_firstUVSetNameIndex);
	}

	// FbxMesh::GetPolygonCount();
	// Meshが持っているポリゴン数を取得する

	const int l_polygonCount = a_fbxMesh->GetPolygonCount();

	// 出力先のModelMeshを初期化する
	a_modelMesh.m_modelVertexList.clear();
	a_modelMesh.m_indexList.clear	   ();

	for (int l_polygonIndex = 0; l_polygonIndex < l_polygonCount; ++l_polygonIndex)
	{
		// 指定Polygonが持つ頂点数を取得する
		// FbxMesh::GetPolygonSize(取得したいPolygonのIndex)

		const int l_polygonVertexCount = a_fbxMesh->GetPolygonSize(l_polygonIndex);

		// LoadStaticModelFile()側でTriangulate済みなので、
		// ここでは必ず三角形のはず
		if (l_polygonVertexCount != k_triangleVertexCount)
		{
			assert(false && "三角形後のFBXに三角形ではないPolygonが含まれているため、ModelMeshの作成に失敗しました。");
			return false;
		}

		for (int l_polygonVertexIndex = 0; l_polygonVertexIndex < l_polygonVertexCount; ++l_polygonVertexIndex)
		{
			// Polygon頂点が参照しているControlPointIndexを取得する
			// FbxMesh::GetPolygonVertex(取得したいPolygonのIndex、
			//							 Polygon内の頂点Index);

			const int l_controlPointIndex = a_fbxMesh->GetPolygonVertex(l_polygonIndex, l_polygonVertexIndex);

			Struct::ModelVertex l_modelVertex = {};

			// ControlPointIndexからPositionを取得する
			l_modelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_controlPointIndex);

			// Polygon単位の頂点情報からNormalを取得する
			l_modelVertex.m_normal = FetchVertexNormal(a_fbxMesh, l_polygonIndex, l_polygonVertexIndex);

			// Polygon単位の頂点座標からUVを取得する
			l_modelVertex.m_uv = FetchVertexUV(a_fbxMesh, l_polygonIndex, l_polygonVertexIndex, l_uvSetName);

			a_modelMesh.m_modelVertexList.emplace_back(l_modelVertex);

			// 今は1頂点追加するたびに、その頂点番号をIndexとして追加する
			// 重複頂点の整理はこの後のmeshoptimizerライブラリで行う
			const auto l_index = static_cast<std::uint32_t>(a_modelMesh.m_indexList.size());

			a_modelMesh.m_indexList.emplace_back(l_index);
		}
	}

	return true;
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::StaticModelFBXLoader::FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、頂点座標の取得に失敗しました。");
		return {};
	}

	// ControlPointから頂点座標を取得する
	// FBX内の頂点座標を取得する
	// FbxMesh::GetControlPointAt(取得したいControlPointのIndex);

	const auto& l_fbxPosition = a_fbxMesh->GetControlPointAt(a_controlPointIndex);

	return ConvertFbxVector4ToVector3(l_fbxPosition);
}
FWK::TypeAlias::Math::Vector3 FWK::Graphics::StaticModelFBXLoader::FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、法線の取得に失敗しました。");
		return {};
	}

	FbxVector4 l_fbxNormal = {};

	// Polygon内の頂点に対応する法線を取得する
	// FbxMesh::GetPolygonVertexNormal(取得したいPolygonのIndex、
	//								   Polygon内の頂点Index、
	//								   取得した法線の格納先);

	if (!a_fbxMesh->GetPolygonVertexNormal(a_polygonIndex, a_polygonVertexIndex, l_fbxNormal))
	{
		assert(false && "Polygon頂点の法線取得に失敗しました。");
		return {};
	}

	return ConvertFbxVector4ToVector3(l_fbxNormal);
}
FWK::TypeAlias::Math::Vector2 FWK::Graphics::StaticModelFBXLoader::FetchVertexUV(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex, const char* a_uvSetName) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、UVの取得に失敗しました。");
		return {};
	}

	if (!a_uvSetName) { return {}; }

	FbxVector2 l_fbxUV	    = {};
	bool	   l_isUnmapped = false;

	// FbxMesh::GetPolygonVertexUV(取得したいPolygonのIndex、
	//							   Polygon内の頂点Index、
	//							   使用するUVSet名、
	//							   取得したいUVの格納先、
	//							   UVが割り当てられていないかどうかの取得先);

	if (!a_fbxMesh->GetPolygonVertexUV(a_polygonIndex,
									   a_polygonVertexIndex,
									   a_uvSetName,
									   l_fbxUV,
									   l_isUnmapped))
	{
		assert(false && "Polygon頂点のUV取得に失敗しました。");
		return {};
	}

	// UVSetは存在するが、このPolygon頂点にUVが割り当てられていない場合はreturn
	if (l_isUnmapped) { return {}; }

	auto l_uv = ConvertFbxVector2ToVector2(l_fbxUV);

	// Blenderから読み込んだ時UV座標が反転しているので
	// 最大値から現在のUV値を引く
	l_uv.y = k_uvCoordinateMax - l_uv.y;
	
	return l_uv;
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::StaticModelFBXLoader::ConvertFbxVector4ToVector3(const FbxVector4& a_fbxVector) const
{
	return
	{
		static_cast<float>(a_fbxVector[k_fbxVectorXIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorYIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorZIndex])
	};
}
FWK::TypeAlias::Math::Vector2 FWK::Graphics::StaticModelFBXLoader::ConvertFbxVector2ToVector2(const FbxVector2& a_fbxVector) const
{
	return
	{
		static_cast<float>(a_fbxVector[k_fbxVectorXIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorYIndex])
	};
}