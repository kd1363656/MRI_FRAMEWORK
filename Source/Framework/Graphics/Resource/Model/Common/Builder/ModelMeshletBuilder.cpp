#include "ModelMeshletBuilder.h"

bool FWK::Graphics::ModelMeshletBuilder::BuildModelMeshletData(Struct::ModelMesh& a_modelMesh, Struct::ModelMeshletData& a_modelMeshletData) const
{
	if (a_modelMesh.m_modelVertexList.empty())
	{
		assert(false && "ModelMeshの頂点リストが空のため、ModelMeshletDataの作成に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_indexList.empty())
	{
		assert(false && "ModelMeshのインデックスリストが空のため、ModelMeshletDataの作成に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_indexList.size() % Constant::k_triangleVertexCount != k_noIndexRemainder)
	{
		assert(false && "ModelMeshのインデックス数が3の倍数ではないため、ModelMeshletDataの作成に失敗しました。");
		return false;
	}

	// 念のため初期化
	a_modelMeshletData = {};

	const auto& l_sourceVertexList = a_modelMesh.m_modelVertexList;
	const auto& l_sourceIndexList  = a_modelMesh.m_indexList;

	std::vector<unsigned int> l_remapList(l_sourceIndexList.size());

	// 頂点重複を取り除くためリマップ情報を作成する
	// meshopt_generateVertexRemap(リマップ情報の格納先、
	//							   元インデックス配列、
	//							   元インデックス数、
	//							   元頂点配列、	
	//							   元頂点数、
	//							   1頂点あたりのバイト数);

	const auto l_uniqueVertexCount = meshopt_generateVertexRemap(l_remapList.data(),
																 l_sourceIndexList.data(),
																 l_sourceIndexList.size(),
																 l_sourceVertexList.data(),
																 l_sourceVertexList.size(),
																 sizeof(Struct::ModelVertex));

	std::vector<Struct::ModelVertex> l_optimizedVertexList(l_uniqueVertexCount);
	std::vector<unsigned int>		 l_optimizedIndexList (l_sourceIndexList.size());

	// リマップ情報を使って、インデックス配列を作り直す
	// meshopt_remapIndexBuffer(変換後インデックス配列、
	//							元インデックス配列、
	//							元インデックス数、
	//							リマップ情報);

	meshopt_remapIndexBuffer(l_optimizedIndexList.data(),
							 l_sourceIndexList.data(),
							 l_sourceIndexList.size(),
							 l_remapList.data());

	// リマップ情報を使って、重複削除後の頂点配列を作り直す
	// meshopt_remapVertexBuffer(変換後頂点配列、
	//							 元頂点配列、
	//							 元頂点数、
	//							 1頂点あたりのバイト数、
	//							 リマップ情報);

	meshopt_remapVertexBuffer(l_optimizedVertexList.data(),
							  l_sourceVertexList.data(),
							  l_sourceVertexList.size(),
							  sizeof(Struct::ModelVertex),
							  l_remapList.data());

	// 頂点キャッシュ効率が良くなるようにインデックス配列を並び替える
	// meshopt_optimizeVertexCache(変換後インデックス配列、
	//							   元インデックス配列、
	//							   インデックス数、
	//								頂点数);

	meshopt_optimizeVertexCache(l_optimizedIndexList.data(),
								l_optimizedIndexList.data(),
								l_optimizedIndexList.size(),
								l_optimizedVertexList.size());

	// メッシュレット作成に必要な最大メッシュレット数を取得する
	// meshopt_buildMeshletsBound(インデックス数、
	//							 1メッシュレットの最大頂点数、
	//							 1メッシュレットの最大三角形数);

	const auto l_meshletBound = meshopt_buildMeshletsBound(l_optimizedIndexList.size(), k_maxMeshletVertexCount, k_maxMeshletTriangleCount);

	std::vector<meshopt_Meshlet> l_meshoptMeshletList(l_meshletBound);

	a_modelMeshletData.m_uniqueVertexIndexList.resize(l_meshletBound * k_maxMeshletVertexCount);
	a_modelMeshletData.m_primitiveIndexList.resize	 (l_meshletBound * k_maxMeshletTriangleCount * Constant::k_triangleVertexCount);

	// メッシュレットを作成する
	// meshopt_buildMeshlets(メッシュレット配列、
	//						 メッシュレットごとのユニーク頂点インデックス配列、
	//						 メッシュレットごとの三角形インデックス配列、
	//						 インデックス配列、
	//					     インデックス数、
	//						 頂点座標配列の先頭アドレス、
	//					     頂点数、
	//						 頂点座標配列の1要素あたりのバイト数、
	//						 1メッシュレットの最大頂点数、
	//						 1メッシュレットの最大三角形数、
	//						 コーンカリング用の重み);

	const auto l_meshletCount = meshopt_buildMeshlets(l_meshoptMeshletList.data(),
													  a_modelMeshletData.m_uniqueVertexIndexList.data(),
													  a_modelMeshletData.m_primitiveIndexList.data(),
													  l_optimizedIndexList.data(),
													  l_optimizedIndexList.size(),
													  &l_optimizedVertexList.front().m_position.x,
													  l_optimizedVertexList.size(),
													  sizeof(Struct::ModelVertex),
													  k_maxMeshletVertexCount,
													  k_maxMeshletTriangleCount,
													  k_meshletConeWeight);

	if (l_meshletCount == k_emptyMeshletCount)
	{
		assert(false && "ModelMeshletの作成数が0のため、ModelMeshletDataの作成に失敗しました。");
		return false;
	}

	const auto& l_lastMeshoptMeshlet = l_meshoptMeshletList[l_meshletCount - k_lastElementOffset];
	
	a_modelMeshletData.m_uniqueVertexIndexList.resize(l_lastMeshoptMeshlet.vertex_offset   + l_lastMeshoptMeshlet.vertex_count);
	a_modelMeshletData.m_primitiveIndexList.resize   (l_lastMeshoptMeshlet.triangle_offset + l_lastMeshoptMeshlet.triangle_count * Constant::k_triangleVertexCount);

	a_modelMeshletData.m_meshletList.resize(l_meshletCount);

	for (std::size_t l_meshletIndex = 0ULL; l_meshletIndex < l_meshletCount; ++l_meshletIndex)
	{
		const auto& l_meshoptMeshlet = l_meshoptMeshletList[l_meshletIndex];

		auto& l_modelMeshlet = a_modelMeshletData.m_meshletList[l_meshletIndex];

		l_modelMeshlet.m_vertexOffset   = l_meshoptMeshlet.vertex_offset;
		l_modelMeshlet.m_triangleOffset = l_meshoptMeshlet.triangle_offset;

		l_modelMeshlet.m_vertexCount   = l_meshoptMeshlet.vertex_count;
		l_modelMeshlet.m_triangleCount = l_meshoptMeshlet.triangle_count;
	}

	// メッシュレットは最適化後の頂点・インデックスを参照するため、
	// ModelMesh側も同じ最適化後のデータへ差し替える
	a_modelMesh.m_modelVertexList = std::move(l_optimizedVertexList);
	a_modelMesh.m_indexList		  = std::move(l_optimizedIndexList);

	return true;
}