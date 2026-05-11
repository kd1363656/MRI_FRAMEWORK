#include "StaticModelFBXLoader.h"

bool FWK::Graphics::StaticModelFBXLoader::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const
{
	// 出力先のModelDataを初期化する
	// 前回読み込み結果が残っていると、今回のFBX結果と混ざるため最初に空にする
	a_modelData = {};

	// FBXファイル全体をufbx_sceneとして読み込む
	auto* l_fbxScene = LoadFBXScene(a_filePath);

	if (!l_fbxScene)
	{
		assert(false && "FBXシーンの読み込みに失敗したため、StaticModelファイルの読み込みに失敗しました。");
		return false;
	}

	if (!ExtractModelData(l_fbxScene, a_modelData))
	{
		assert(false && "FBXシーンからModelDataの抽出に失敗しました。");
		
		DestroyFBXScene(l_fbxScene);

		return false;
	}

	DestroyFBXScene(l_fbxScene);

	return true;
}

bool FWK::Graphics::StaticModelFBXLoader::ExtractModelData(const ufbx_scene* a_fbxScene, Struct::ModelData& a_modelData) const
{
	if (!a_fbxScene) 
	{
		assert(false && "ufbx_sceneがnullptrのため、ModelDataの抽出に失敗しました。");
		return false;
	}

	// ufbx_scene::meshesには、FBX内に存在する全メッシュが入っている
	// メッシュが0個の場合、ModelDataとして描画できる情報がないため失敗扱いにする
	if (a_fbxScene->meshes.count == Constant::k_emptyModelMeshCount)
	{
		assert(false && "FBXシーン内にMeshが存在しないため、ModelDataの抽出に失敗しました。");
		return false;
	}

	for (std::size_t l_meshIndex = 0ULL; l_meshIndex < a_fbxScene->meshes.count; ++l_meshIndex)
	{
		const auto* l_fbxMesh = a_fbxScene->meshes.data[l_meshIndex];

		if (!l_fbxMesh) { continue; }

		Struct::ModelMesh l_modelMesh = {};

		// ufbx_mesh 1つを、自作フレームワーク側のModelMesh 1つへ変換する
		if (!ExtractModelMesh(l_fbxMesh, l_modelMesh))
		{
			assert(false && "ufbx_meshからModelMeshの抽出に失敗しました。");
			return false;
		}

		// 頂点またはインデックスが空のメッシュは描画できないため、ModelDataへ登録しない
		if (l_modelMesh.m_modelVertexList.empty()) { continue; }
		if (l_modelMesh.m_indexList.empty())	   { continue; }

		a_modelData.m_modelMeshList.emplace_back(std::move(l_modelMesh));
	}

	if (a_modelData.m_modelMeshList.empty())
	{
		assert(false && "有効なModelMeshが存在しないため、ModelDataの抽出に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::StaticModelFBXLoader::ExtractModelMesh(const ufbx_mesh* a_fbxMesh, Struct::ModelMesh& a_modelMesh) const
{
	if (!a_fbxMesh)
	{
		assert(false && "ufbx_meshがnullptrのため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	// Faceはポリゴン面のこと
	// Faceが存在しないMeshは、三角形へ変換する元データがないため失敗扱いにする
	if (a_fbxMesh->faces.count == Constant::k_emptyModelMeshCount)
	{
		assert(false && "ufbx_mesh内にFaceが存在しないため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	// max_face_trianglesは、1つのFaceを三角形化したときに必要になる最大三角形数
	// これが0の場合、三角形化できるFaceがない
	if (a_fbxMesh->max_face_triangles == Constant::k_emptyModelMeshCount)
	{
		assert(false && "三角形化できるFaceが存在しないため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	// ufbx_triangulate_face()は、三角形化した頂点インデックスを配列へ書き込む
	// 1三角形は3頂点なので、最大三角形数 * 3の作業用配列を用意する
	const auto l_triangleIndexListSize = a_fbxMesh->max_face_triangles * k_triangleVertexCount;

	std::vector<std::uint32_t> l_triangleIndexList = {};

	l_triangleIndexList.resize(l_triangleIndexListSize);

	for (std::size_t l_faceIndex = 0ULL; l_faceIndex < a_fbxMesh->faces.count; ++l_faceIndex)
	{
		const auto& l_fbxFace = a_fbxMesh->faces.data[l_faceIndex];

		// FBXのFaceは四角形以上の場合もあるため、描画しやすい三角形リストへ変換する
		// ufbx_triangulate_face(三角形化した頂点インデックスの書き込み先、
		//						 書き込み先配列の要素数、
		//						 三角形化するメッシュ、
		//						 三角形化するFace);

		const auto l_triangleCount = ufbx_triangulate_face(l_triangleIndexList.data(),
														   l_triangleIndexList.size(),
														   a_fbxMesh,
														   l_fbxFace);

		for (std::size_t l_triangleIndex = 0ULL; l_triangleIndex < l_triangleCount; ++l_triangleIndex)
		{
			for (std::uint32_t l_vertexIndex = 0U; l_vertexIndex < k_triangleVertexCount; ++l_vertexIndex)
			{
				// l_triangleIndexListには、三角形化後のufbx側頂点インデックスが入っている
				// 三角形番号 * 3 + 頂点番号で、現在処理している三角形の頂点インデックスを取り出す
				const auto l_indexOffset    = (l_triangleIndex * k_triangleVertexCount) + l_vertexIndex;
				const auto l_fbxVertexIndex = l_triangleIndexList[l_indexOffset];

				Struct::ModelVertex l_modelVertex = {};

				// ufbx_meshから頂点座標、法線、UVを取得して、自作ModelVertexへコピーする
				l_modelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_fbxVertexIndex);
				l_modelVertex.m_normal   = FetchVertexNormal  (a_fbxMesh, l_fbxVertexIndex);
				l_modelVertex.m_uv       = FetchVertexUV      (a_fbxMesh, l_fbxVertexIndex);

				// DirectXのUV座標に合わせるため、V座標を反転する
				// BlenderなどのDCCツールとDirectXでは、テクスチャの上下方向の扱いが異なる場合がある
				l_modelVertex.m_uv.y = k_uvCoordinateMax - l_modelVertex.m_uv.y;

				// 今は重複頂点削除をまだ行わないため、三角形の頂点をそのまま追加する
				// Indexは追加した頂点の順番をそのまま指す
				a_modelMesh.m_modelVertexList.emplace_back(l_modelVertex);
				a_modelMesh.m_indexList.emplace_back      (static_cast<std::uint32_t>(a_modelMesh.m_indexList.size()));
			}
		}
	}

	if (a_modelMesh.m_modelVertexList.empty())
	{
		assert(false && "ModelMeshの頂点リストが空のため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_indexList.empty())
	{
		assert(false && "ModelMeshのインデックスリストが空のため、ModelMeshの抽出に失敗しました。");
		return false;
	}

	return true;
}