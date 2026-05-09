#include "StaticModelFBXLoader.h"

bool FWK::Graphics::StaticModelFBXLoader::LoadStaticModelFile(const std::filesystem::path& a_filePath, FbxManager* a_fbxManager) const
{
	auto* l_fbxScene = ImportScene(a_filePath, a_fbxManager);

	if (!l_fbxScene)
	{
		assert(false && "FbxSceneへのImportに失敗したため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	// シーンからルートノードを取得
	auto* l_rootNode = l_fbxScene->GetRootNode();

	if (!l_rootNode)
	{
		assert(false && "RootNodeが存在しないため、StaticModelの読み込みに失敗しました。");
		
		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	// ルートノードを介してメッシュノードの数をカウント
	const int l_meshCount = RecursiveCountMeshNode(l_rootNode);

	if (l_meshCount <= k_emptyMeshCount)
	{
		assert(false && "StaticModelとして使用できるMeshNodeが存在しないため、StaticModelの読み込みに失敗しました。");
		
		l_fbxScene->Destroy();
		l_fbxScene = nullptr;

		return false;
	}

	l_fbxScene->Destroy();
	l_fbxScene = nullptr;

	return true;
}

int FWK::Graphics::StaticModelFBXLoader::RecursiveCountMeshNode(FbxNode* a_fbxNode) const
{
	if (!a_fbxNode)
	{
		assert(false && "FbxNodeが無効のため、MeshNode数の取得に失敗しました。");
		return k_emptyMeshCount;
	}

	int l_meshCount = k_emptyMeshCount;

	auto* l_fbxNodeAttribute = a_fbxNode->GetNodeAttribute();

	if (l_fbxNodeAttribute)
	{
		// FbxNodeAttribute::GetAttributeType();
		// FbxNodeが持っている属性の種類を取得する

		if (l_fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			++l_meshCount;
		}
	}

	// FbxNode::GetChildCount();
	// FbxNodeが持っている子ノード数を取得する

	const int l_childNodeCount = a_fbxNode->GetChildCount();

	for (int l_childNodeIndex = 0; l_childNodeIndex < l_childNodeCount; ++l_childNodeIndex)
	{
		// FbxNode::GetChild(取得したい子ノードのIndex)
		auto* l_childNode = a_fbxNode->GetChild(l_childNodeIndex);

		l_meshCount += RecursiveCountMeshNode(l_childNode);
	}

	return l_meshCount;
}