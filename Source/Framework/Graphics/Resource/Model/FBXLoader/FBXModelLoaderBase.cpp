#include "FBXModelLoaderBase.h"

ufbx_scene* FWK::Graphics::FBXModelLoaderBase::LoadFBXScene(const std::filesystem::path& a_filePath) const
{
	// FBXファイルとして読み込めるパスか確認する
	// 存在しないファイルや.fbx以外のファイルをufbxへ渡さないための事前チェック
	if (!Utility::File::CanLoadFilePath(a_filePath, Constant::k_lowerFBXExtension))
	{
		assert(false && "FBXファイルが読み込める形式ではありません、FBXシーンの読み込みに失敗しました。");
		return nullptr;
	}

	// ufbx_load_optsは、ufbxでFBXを読み込むときの設定
	// 今は特別な読み込み設定を使わないため、空初期化でデフォルト設定で読み込む

	const auto& l_loadOptions = CreateFBXLoadOptions();
	
	// ufbx_errorは、読み込み失敗時の詳細情報を受け取るための変数
	// ufbx_load_file()が失敗した場合、この中にエラー理由が入る

	ufbx_error l_error = {};

	const auto& l_filePath = a_filePath.string();

	// 成功するとufbx_scene*が返り、失敗するとnullptrが返る
	// ufbx_load_file(読み込むFBXファイルパス、
	//				  読み込み設定、
	//				  エラー情報の書き込み先);

	auto* l_fbxScene = ufbx_load_file(l_filePath.c_str(), &l_loadOptions, &l_error);

	if (!l_fbxScene)
	{
#if defined(_DEBUG)
		std::array<char, k_errorTextBufferSize> l_errorText = {};

		// ufbx_errorはそのままだと読みにくいため、人が読める文字列へ変換する
		// ufbx_format_error(エラー文字列の書き込み先、
		//					 書き込み先バッファサイズ、
		//					 ufbx_load_fileで取得したエラー情報);

		ufbx_format_error (l_errorText.data(), l_errorText.size(), &l_error);
		OutputDebugStringA(l_errorText.data());

		assert(false && "ufbx_load_fileによるFBXシーンの読み込みに失敗しました。");
#endif

		return nullptr;
	}

	return l_fbxScene;
}

void FWK::Graphics::FBXModelLoaderBase::DestroyFBXScene(ufbx_scene* a_fbxScene) const
{
	if (!a_fbxScene) { return; }

	// ufbx_load_file()で作成されたufbx_sceneは、使い終わったらufbx_free_scene()で解放する
	// 今回はModelDataへ必要な情報をコピーした後、StaticModelFBXLoader側でこの関数を呼ぶ

	ufbx_free_scene(a_fbxScene);
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoaderBase::FetchVertexPosition(const ufbx_mesh* a_fbxMesh, const ufbx_node* a_fbxNode, const std::uint32_t a_vertexIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "ufbx_meshがnullptrのため、頂点座標の取得に失敗しました。");
		return {};
	}

	if (!a_fbxNode)
	{
		assert(false && "ufbx_meshがnullptrのため、頂点座標の取得に失敗しました。");
		return {};
	}

	// ufbx_get_vertex_vec3()で、指定した頂点インデックスの座標を取得する
	// ufbx_mesh::vertex_positionには、FBX内の頂点座標データが入っている
	const auto& l_position = ufbx_get_vertex_vec3(&a_fbxMesh->vertex_position, a_vertexIndex);

	// 指定した頂点インデックスの座標を取得する
	// ufbx_get_vertex_vec3(attribute spaceからworld spaceへ変換するための行列、
	//						変換したい頂点座標);
	// node->geometry_to_worldを使うことで、FBX内のNode Transform / Geometry Transformを頂点へ反映する
	const auto& l_worldPosition = ufbx_transform_position(&a_fbxNode->geometry_to_world, l_position);
	
	return ConvertUFBXVector3ToVector3(l_position);
}
FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoaderBase::FetchVertexNormal(const ufbx_mesh* a_fbxMesh, const ufbx_node* a_fbxNode, const std::uint32_t a_vertexIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "ufbx_meshがnullptrのため、法線の取得に失敗しました。");
		return {};
	}

	if (!a_fbxNode)
	{
		assert(false && "ufbx_meshがnullptrのため、法線の取得に失敗しました。");
		return {};
	}

	// FBXによっては法線が入っていない場合がある
	// その場合は今は空のVector3を返し、読み込み自体は続行する
	if (!a_fbxMesh->vertex_normal.exists) { return {}; }

	// ufbx_get_vertex_vec3()で、指定した頂点インデックスの法線を取得する
	// ufbx_mesh::vertex_normalには、FBX内の法線データが入っている
	const auto& l_normal = ufbx_get_vertex_vec3(&a_fbxMesh->vertex_normal, a_vertexIndex);

	// ufbx_matrix_for_normals(法線変換用の元になる行列);
	// スケールが入っている行列で法線をそのまま変換するとき向きが崩れる可能性があるため、法線専用の変換行列を作成する
	const auto& l_normalMatrix = ufbx_matrix_for_normals(&a_fbxNode->geometry_to_world);

	// ufbx_transform_direction(法線変換用の行列、
	//						    変換したい法線);
	// 法線は位置ではなく方向なので、平行移動の影響を受けないDirection変換を使う
	const auto& l_worldNormal = ufbx_vec3_normalize(ufbx_transform_direction(&l_normalMatrix, l_normal));

	return ConvertUFBXVector3ToVector3(l_worldNormal);
}

FWK::TypeAlias::Math::Vector2 FWK::Graphics::FBXModelLoaderBase::FetchVertexUV(const ufbx_mesh* a_fbxMesh, const std::uint32_t a_vertexIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "ufbx_meshがnullptrのため、UVの取得に失敗しました。");
		return {};
	}

	// FBXによってはUVが入っていない場合がある
	// その場合は今は空のVector2を返し、読み込み自体は続行する
	if (!a_fbxMesh->vertex_uv.exists) { return {}; }

	// ufbx_get_vertex_vec2()で、指定した頂点インデックスのUVを取得する
	// ufbx_mesh::vertex_uvには、FBX内のUV座標データが入っている
	const auto& l_uv = ufbx_get_vertex_vec2(&a_fbxMesh->vertex_uv, a_vertexIndex);

	auto l_convertedUV = ConvertUFBXVector2ToVector2(l_uv);

	// DirectXのUV座標に合わせるため、V座標を反転する
	// BlenderなどのDCCツールとDirectXでは、テクスチャの上下方向の扱いが異なる場合がある
	l_convertedUV.y = k_uvCoordinateMax - l_convertedUV.y;

	return l_convertedUV;
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoaderBase::ConvertUFBXVector3ToVector3(const ufbx_vec3& a_fbxVector) const
{
	// ufbx_vec3はdouble系の値を持つため、自作フレームワークのVector3で使うfloatへ変換する
	return TypeAlias::Math::Vector3
	(
		static_cast<float>(a_fbxVector.x),
		static_cast<float>(a_fbxVector.y),
		static_cast<float>(a_fbxVector.z)
	);
}
FWK::TypeAlias::Math::Vector2 FWK::Graphics::FBXModelLoaderBase::ConvertUFBXVector2ToVector2(const ufbx_vec2& a_fbxVector) const
{
	// ufbx_vec2はdouble系の値を持つため、自作フレームワークのVector2で使うfloatへ変換する
	return TypeAlias::Math::Vector2
	(
		static_cast<float>(a_fbxVector.x),
		static_cast<float>(a_fbxVector.y)
	);
}

ufbx_load_opts FWK::Graphics::FBXModelLoaderBase::CreateFBXLoadOptions() const
{
	// ufbx_load_optsは、ufbxでFBXを読み込むときの設定
	// StaticModelFBXLoader / AnimationModelFBXLoaderの両方で同じ読む込み補正を使うため、
	// FBXModelLoaderBase側で共通化する

	ufbx_load_opts l_loadOptions = {};

	// use_root_transform;
	// trueにすると、root_transformで指定した読み込み時のルート変換を使用する
	l_loadOptions.use_root_transform = true;

	// ues_root_transform.translation;
	// 今回は読み込み時に位置移動はしないため、UFBX側のゼロベクトルを使う
	l_loadOptions.root_transform.translation = ufbx_zero_vec3;

	// root_transform.rotation
	// Blender / FBX側の向きをエンジン側に合わせるため、
	// X軸 -90度、Y軸 180度、Z軸 0度の回転をQuaternionへ変換して設定する
	// ufbx_euler_to_quat(回転角度XYZ,
	//					  Euler回転の適用順);

	l_loadOptions.root_transform.rotation = ufbx_euler_to_quat
	(
		ufbx_vec3
		{
			k_modelImportRotationXDegrees,
			k_modelImportRotationYDegrees,
			k_modelImportRotationZDegrees
		},
		UFBX_ROTATION_ORDER_XYZ
	);

	// root_transform.scale;
	// Blnder側で0.01倍したい読み込み補正を、FBX読み込み時の共通スケールとして提供する
	l_loadOptions.root_transform.scale = ufbx_vec3
	(
		k_modelImportScale,
		k_modelImportScale,
		k_modelImportScale
	);

	// normalize_normals;
	// 読み込み後んお法線を正規化する
	// スケールや回転補正後に法線の長さが崩れる可能性を減らすために有効化する
	l_loadOptions.normalize_normals = true;

	return l_loadOptions;
}