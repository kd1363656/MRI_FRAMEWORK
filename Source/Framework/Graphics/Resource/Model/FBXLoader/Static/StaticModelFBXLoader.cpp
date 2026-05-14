#include "StaticModelFBXLoader.h"

bool FWK::Graphics::StaticModelFBXLoader::LoadStaticModelFile(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath) const
{
	const auto& l_staticModelRecord = a_staticModelRecord.lock();

	if (!l_staticModelRecord)
	{
		assert(false && "StaticModelRecordが無効のため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	auto& l_modelData = l_staticModelRecord->m_modelData;

	// ModelDataはコピー代入禁止のため、保持しているModelMeshリストだけを空にする
	l_modelData.m_modelMeshList.clear();

	// FBXファイル全体をufbx_sceneとして読み込む
	auto* l_fbxScene = LoadFBXScene(a_filePath);

	if (!l_fbxScene)
	{
		assert(false && "FBXシーンの読み込みに失敗したため、StaticModelファイルの読み込みに失敗しました。");
		return false;
	}

	// モデルデータをシーンから抽出
	if (!ExtractModelData(l_fbxScene, l_modelData))
	{
		assert(false && "FBXシーンからModelDataの抽出に失敗しました。");
		
		// 安全のためシーン情報の破棄
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

	for (auto l_meshIndex = 0ULL; l_meshIndex < a_fbxScene->meshes.count; ++l_meshIndex)
	{
		const auto* l_fbxMesh = a_fbxScene->meshes.data[l_meshIndex];

		if (!l_fbxMesh) { continue; }

		std::vector<Struct::ModelMesh> l_modelMeshList = {};

		// ufbx_mesh 1つを、自作フレームワーク側のModelMesh 1つへ変換する
		// 1つのufbx_meshに複数のMaterialがある場合、MaterialごとにModelMeshを分割する
		if (!ExtractModelMeshList(l_fbxMesh, l_modelMeshList))
		{
			assert(false && "ufbx_meshからModelMeshリストの抽出に失敗しました。");
			return false;
		}

		for (auto& l_modelMesh : l_modelMeshList)
		{
			// 頂点またはインデックスが空のメッシュは描画できないため、ModelDataへ登録しない
			if (l_modelMesh.m_modelVertexList.empty()) { continue; }
			if (l_modelMesh.m_indexList.empty())	   { continue; }

			a_modelData.m_modelMeshList.emplace_back(std::move(l_modelMesh));
		}
	}

	if (a_modelData.m_modelMeshList.empty())
	{
		assert(false && "有効なModelMeshが存在しないため、ModelDataの抽出に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Graphics::StaticModelFBXLoader::ExtractModelMeshList(const ufbx_mesh* a_fbxMesh, std::vector<Struct::ModelMesh>& a_modelMeshList) const
{
	a_modelMeshList.clear();

	if (!a_fbxMesh)
	{
		assert(false && "ufbx_meshがnullptrのため、ModelMeshリストの抽出に失敗しました。");
		return false;
	}

	// MaterialがないMeshの場合は、MaterialなしのModelMeshとして1つだけ作成する
	if (a_fbxMesh->materials.count == Constant::k_emptyModelMeshCount)
	{
		Struct::ModelMesh l_modelMesh = {};

		if (!ExtractModelMeshByMaterial(a_fbxMesh, k_invalidMaterialIndex, l_modelMesh))
		{
			assert(false && "MaterialなしModelMeshの抽出に失敗しました。");
			return false;
		}

		if (!l_modelMesh.m_modelVertexList.empty() && !l_modelMesh.m_indexList.empty())
		{
			// Materialが存在しないため、AssetData / RuntimeDataは初期値のままにする
			l_modelMesh.m_modelMaterial = {};

			a_modelMeshList.emplace_back(std::move(l_modelMesh));
		}

		return true;
	}

	for (std::size_t l_materialIndex = 0ULL; l_materialIndex < a_fbxMesh->materials.count; ++l_materialIndex)
	{
		Struct::ModelMesh l_modelMesh = {};

		// 現在のMaterialIndexを使用しているFaceだけを集めて、1つのModelMeshにする
		if (!ExtractModelMeshByMaterial(a_fbxMesh, l_materialIndex, l_modelMesh))
		{
			assert(false && "Material別ModelMeshの抽出に失敗しました。");
			return false;
		}

		// このMaterialを使用しているFaceがなければ描画対象にしない
		if (l_modelMesh.m_modelVertexList.empty()) { continue; }
		if (l_modelMesh.m_indexList.empty())	   { continue; }

		const auto* l_fbxMaterial = a_fbxMesh->materials.data[l_materialIndex];

		// FBXから取得したMaterial情報のうち、.assetへ保存してよいデータだけを設定する
		ExtractModelMaterial(l_fbxMaterial, l_modelMesh.m_modelMaterial.m_modelMaterialAssetData);

		// Runtime情報はTextureSystem登録後に決めるため、FBXLoaderでは必ず初期値のままにする
		l_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData = {};

		a_modelMeshList.emplace_back(std::move(l_modelMesh));
	}

	return true;
}

bool FWK::Graphics::StaticModelFBXLoader::ExtractModelMeshByMaterial(const ufbx_mesh* a_fbxMesh, const std::size_t& a_materialIndex, Struct::ModelMesh& a_modelMesh) const
{
	// モデルメッシュの初期化
	a_modelMesh.m_modelVertexList.clear();
	a_modelMesh.m_indexList.clear	   ();
	
	if (!a_fbxMesh)
	{
		assert(false && "ufbx_meshがnullptrのため、Material別ModelMeshの抽出に失敗しました。");
		return false;
	}

	// Faceはポリゴン面のこと
	// Faceが存在しないMeshは、三角形へ変換する元データがないため失敗扱いにする
	if (a_fbxMesh->faces.count == Constant::k_emptyModelMeshCount)
	{
		assert(false && "三角形化できるFaceが存在しないため、Material別ModelMeshの抽出に失敗しました。");
		return false;
	}

	// max_face_trianglesは、1つのFaceを三角形化したときに必要になる最大三角形数
	// これが0の場合、三角形化できるFaceがない
	if (a_fbxMesh->max_face_triangles == Constant::k_emptyModelMeshCount)
	{
		assert(false && "三角形化できるFaceが存在しないため、Material別ModelMeshの抽出に失敗しました。");
		return false;
	}

	if (a_materialIndex				   != k_invalidMaterialIndex &&
		a_fbxMesh->face_material.count != a_fbxMesh->faces.count)
	{
		assert(false && "face_material数とFace数が一致しないため、Material別ModelMeshの抽出に失敗しました。");
		return false;
	}

	// ufbx_triangulate_face()は、三角形化した頂点インデックスを配列へ書き込む
	// 1三角形は3頂点なので、最大三角形数 * 3の作業用配列を用意する
	const auto& l_triangleIndexListSize = a_fbxMesh->max_face_triangles * k_triangleVertexCount;

	std::vector<std::uint32_t> l_triangleIndexList = {};

	l_triangleIndexList.resize(l_triangleIndexListSize);

	for (auto l_faceIndex = 0ULL; l_faceIndex < a_fbxMesh->faces.count; ++l_faceIndex)
	{
		const auto& l_fbxFace = a_fbxMesh->faces.data[l_faceIndex];

		// k_invalidMaterialIndexの場合は、MaterialなしMeshとして全Faceを対象にする
		// それ以外の場合は、face_material配列を使って、現在処理中のMaterialIndexと一致するFaceだけを対象にする
		if (a_materialIndex != k_invalidMaterialIndex)
		{
			const auto& l_faceMaterialIndex = static_cast<std::size_t>(a_fbxMesh->face_material.data[l_faceIndex]);

			if (l_faceMaterialIndex != a_materialIndex)
			{
				continue;
			}
		}

		// FBXのFaceは四角形以上の場合もあるため、描画しやすい三角形リストへ変換する
		// ufbx_triangulate_face(三角形化した頂点インデックスの書き込み先、
		//						 書き込み先配列の要素数、
		//						 三角形化するメッシュ、
		//						 三角形化するFace);

		const auto l_triangleCount = ufbx_triangulate_face(l_triangleIndexList.data(),
														   l_triangleIndexList.size(),
														   a_fbxMesh,
														   l_fbxFace);

		for (auto l_triangleIndex = 0ULL; l_triangleIndex < l_triangleCount; ++l_triangleIndex)
		{
			for (auto l_vertexIndex = 0U; l_vertexIndex < k_triangleVertexCount; ++l_vertexIndex)
			{
				// l_triangleIndexListには、三角形化後のufbx側頂点インデックスが入っている
				// 三角形番号 * 3 + 頂点番号で、現在処理している三角形の頂点インデックスを取り出す
				const auto l_indexOffset    = (l_triangleIndex * k_triangleVertexCount) + l_vertexIndex;
				const auto l_fbxVertexIndex = l_triangleIndexList[l_indexOffset];

				Struct::ModelVertex l_modelVertex = {};

				// ufbx_meshから頂点座標、法線、UVを取得して、自作ModelVertexへコピーする
				l_modelVertex.m_position = FetchVertexPosition(a_fbxMesh, l_fbxVertexIndex);
				l_modelVertex.m_normal   = FetchVertexNormal  (a_fbxMesh, l_fbxVertexIndex);
				l_modelVertex.m_uv	     = FetchVertexUV      (a_fbxMesh, l_fbxVertexIndex);

				// 今は重複頂点削除をまだ行わないため、三角形の頂点をそのまま追加する
				// Indexは追加した頂点の順番をそのまま示す
				a_modelMesh.m_modelVertexList.emplace_back(l_modelVertex);
				a_modelMesh.m_indexList.emplace_back	  (static_cast<std::uint32_t>(a_modelMesh.m_indexList.size()));
			}
		}
	}

	return true;
}

void FWK::Graphics::StaticModelFBXLoader::ExtractModelMaterial(const ufbx_material* a_fbxMaterial, Struct::ModelMaterialAssetData& a_modelMaterialAssetData) const
{
	a_modelMaterialAssetData = {};

	if (!a_fbxMaterial) { return; }

	// ベースカラー
	{
		// BaseColorはPBR Materialならpbr.base_color,
		auto& l_textureFilePath = a_modelMaterialAssetData.m_baseColorTextureFilePath;

		// 通常のFBX Materialならfbx.diffuse_colorに入っていることが多い
		l_textureFilePath = FetchTextureFilePath(a_fbxMaterial->pbr.base_color);

		if (l_textureFilePath.empty())
		{
			l_textureFilePath = FetchTextureFilePath(a_fbxMaterial->fbx.diffuse_color);
		}
	}

	// 法線
	{
		// NormalMapはPBR Materialならpbr.normal_map、
		// 通常のFBX Materialならfbx.normal_mapに入っていることが多い
		auto& l_textureFilePath = a_modelMaterialAssetData.m_normalTextureFilePath;

		l_textureFilePath = FetchTextureFilePath(a_fbxMaterial->pbr.normal_map);

		if (l_textureFilePath.empty())
		{
			l_textureFilePath = FetchTextureFilePath(a_fbxMaterial->fbx.normal_map);
		}
	}

	// RoughnessはPBR Material側から取得する
	a_modelMaterialAssetData.m_roughnessTextureFilePath = FetchTextureFilePath(a_fbxMaterial->pbr.roughness);

	// MetallicはPBR Material側から取得する
	a_modelMaterialAssetData.m_metallicTextureFilePath = FetchTextureFilePath(a_fbxMaterial->pbr.metalness);
}

std::wstring FWK::Graphics::StaticModelFBXLoader::FetchTextureFilePath(const ufbx_material_map& a_materialMap) const
{
	const auto* l_fbxTexture = a_materialMap.texture;

	if (!l_fbxTexture) { return {}; }

	std::filesystem::path l_textureFilePath = {};

	if (l_fbxTexture->type != UFBX_TEXTURE_FILE) { return {}; }

	// ufbx_texture_typeがUFBX_TEXTURE_FILEの場合、
	// filename / relative_filenameに画像ファイルパスが入っている
	if (l_fbxTexture->relative_filename.length != k_emptyStringLength)
	{
		l_textureFilePath = ConvertUFBXStringToWString(l_fbxTexture->relative_filename);
	}
	else if (l_fbxTexture->filename.length != k_emptyStringLength)
	{
		l_textureFilePath = ConvertUFBXStringToWString(l_fbxTexture->filename);
	}

	if (l_textureFilePath.empty()) { return {}; }

	// 現在のTextureSystemはDDS読み込み方針なので、
	// FBX内のpng等の参照をエンジンで使うddsパスへ変換する
	l_textureFilePath.replace_extension(Constant::k_lowerDDSExtension);

	return l_textureFilePath.wstring();
}
std::wstring FWK::Graphics::StaticModelFBXLoader::ConvertUFBXStringToWString(const ufbx_string& a_fbxString) const
{
	if (!a_fbxString.data) { return {}; }

	if (a_fbxString.length == k_emptyStringLength) { return {}; }

	std::string l_string = {};

	l_string.assign(a_fbxString.data, a_fbxString.length);

	return std::filesystem::path(l_string).wstring();
}