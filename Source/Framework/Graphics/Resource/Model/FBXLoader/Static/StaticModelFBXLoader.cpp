#include "StaticModelFBXLoader.h"

bool FWK::Graphics::StaticModelFBXLoader::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::StaticModelData& a_staticModelData, FbxManager* a_fbxManager) const
{
	auto* l_fbxScene = ImportScene(a_filePath, a_fbxManager);

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneへのImportに失敗したため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	FbxGeometryConverter l_geometryConverter(a_fbxManager);

	// FbxGeometryConverter::Triangulate(三角形化するFbxScene,
	//									 元のMeshを三角形化後のMeshに置き換えるかどうか);

	if (!l_geometryConverter.Triangulate(l_fbxScene, k_isReplaceOriginalMesh))
	{
		assert(false && "FBXのMeshを三角形化できなかったため、StaticModelの読み込みに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	// FBXシーン全体の親になるRootNodeを取得する
	// FbxScene::GetRootNode();

	auto* l_rootNode = l_fbxScene->GetRootNode();

	if (!l_rootNode)
	{
		assert(false && "RootNodeが存在しないため、StaticModelの読み込みに失敗しました。");
		
		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	a_staticModelData.m_modelMeshList.clear();

	if (!RecursiveExtractModelMesh(a_staticModelData, l_rootNode))
	{
		assert(false && "StaticModelとして使用できるMeshNodeが存在しないため、StaticModelの読み込みに失敗しました。");
		
		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	if (a_staticModelData.m_modelMeshList.size() <= k_emptyMeshCount)
	{
		assert(false && "StaticModelとして使用できるModelMeshが存在しないため、StaticModelの読み込みに失敗しました。");

		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	l_fbxScene->Destroy();
	l_fbxScene = nullptr;

	return true;
}

bool FWK::Graphics::StaticModelFBXLoader::RecursiveExtractModelMesh(Struct::StaticModelData& a_staticModelData, FbxNode* a_fbxNode) const
{
	if (!a_fbxNode)
	{
		assert(false && "FbxNodeが無効のため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	auto* l_fbxNodeAttribute = a_fbxNode->GetNodeAttribute();

	if (l_fbxNodeAttribute)
	{
		// FbxNodeが持っている属性の種類を取得する
		// FbxNodeAttribute::GetAttributeType();

		if (l_fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			// FbxNodeが持っているFbxMeshを取得する
			// FbxNode::GetMesh();

			auto* l_fbxMesh = a_fbxNode->GetMesh();

			if (!l_fbxMesh)
			{
				assert(false && "FbxMeshが存在しないため、ModelMeshの抽出に失敗しました。");
				return false;
			}

			Struct::ModelMesh l_modelMesh = {};

			if (!ExtractModelMesh(l_modelMesh, l_fbxMesh))
			{
				assert(false && "FbxMeshからModelMeshの作成に失敗しました。");
				return false;
			}

			a_staticModelData.m_modelMeshList.emplace_back(l_modelMesh);
		}
	}

	// FbxNodeが持っている子ノードを取得する
	// FbxNode::GetChildCount();

	const int l_childNodeCount = a_fbxNode->GetChildCount();

	for (int l_childNodeIndex = 0; l_childNodeIndex < l_childNodeCount; ++l_childNodeIndex)
	{
		// FbxNode::GetChild(取得したい子ノードのIndex);

		auto* l_childNode = a_fbxNode->GetChild(l_childNodeIndex);

		if (!RecursiveExtractModelMesh(a_staticModelData, l_childNode))
		{
			assert(false && "子FbxNodeからModelMeshの抽出に失敗しました。");
			return false;
		}
	}

	return true;
}
bool FWK::Graphics::StaticModelFBXLoader::ExtractModelMesh(Struct::ModelMesh& a_modelMesh, FbxMesh* a_fbxMesh) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、ModelMeshの作成に失敗しました。");
		return false;
	}

	FbxStringList l_uvSetNameList = {};

	// Meshが持っているUVSet名一覧を取得する
	// FbxMesh::GetUVSetNames(UVSet名の格納先);

	a_fbxMesh->GetUVSetNames(l_uvSetNameList);

	const char* l_uvSetName = nullptr;

	if (l_uvSetNameList.GetCount() > k_emptyUVSetNameCount)
	{
		l_uvSetName = l_uvSetNameList.GetStringAt(k_firstUVSetNameIndex);
	}

	// FbxMesh::GetPolygonCount();
	// Meshが持っているポリゴン数を取得する

	const int l_polygonCount = a_fbxMesh->GetPolygonCount();

	a_modelMesh.m_modelVertexList.clear();
	a_modelMesh.m_indexList.clear	   ();

	for (int l_polygonIndex = 0; l_polygonIndex < l_polygonCount; ++l_polygonIndex)
	{
		// 指定Polygonが持つ頂点数を取得する
		// FbxMesh::GetPolygonSize(取得したいPolygonのIndex)

		const int l_polygonVertexCount = a_fbxMesh->GetPolygonSize(l_polygonIndex);

		if (l_polygonVertexCount != k_triangleVertexCount)
		{
			assert(false && "三角形後のFBXに三角形ではないPolygonが含まれているため、ModelMeshの作成に失敗しました。");
			return false;
		}

		for (int l_polygonVertexIndex = 0; l_polygonVertexIndex < l_polygonVertexCount; ++l_polygonVertexIndex)
		{
			// Polygon頂点が参照しているControlPointIndexを取得する
			// FbxMesh::GetPolygonVertex(取得したいPolygonのIndex,
			//							 Polygon内の頂点Index);
			const int l_controlPointIndex = a_fbxMesh->GetPolygonVertex(l_polygonIndex, l_polygonVertexIndex);

			Struct::ModelVertex l_modelVertex = {};

			l_modelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_controlPointIndex);
			l_modelVertex.m_normal   = FetchVertexNormal  (a_fbxMesh, l_polygonIndex, l_polygonVertexIndex);
			l_modelVertex.m_uv		 = FetchVertexUV	  (a_fbxMesh, l_polygonIndex, l_polygonVertexIndex, l_uvSetName);

			a_modelMesh.m_modelVertexList.emplace_back(l_modelVertex);

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

	// FbxMesh::GetControlPointAt(取得したいControlPointのIndex);
	// FBX内の頂点座標を取得する
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

	if (l_isUnmapped) { return {}; }

	return ConvertFbxVector2ToVector2(l_fbxUV);
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
		static_cast<float>(k_fbxVectorXIndex),
		static_cast<float>(k_fbxVectorYIndex)
	};
}