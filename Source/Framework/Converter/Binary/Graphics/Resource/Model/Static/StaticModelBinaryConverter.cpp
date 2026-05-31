#include "StaticModelBinaryConverter.h"

bool FWK::Converter::StaticModelBinaryConverter::LoadStaticModelAsset(Graphics::StaticModelRecord& a_staticModelRecord, const std::filesystem::path& a_filePath)
{
	auto& l_modelData = a_staticModelRecord.GetREFModelData();

	FWK_ASSERT_RETURN_VALUE_IF(!CreateReadMemoryMappedFile(a_filePath), "読み込むためのメモリマップドファイル作成に失敗しました。", false);

	const auto* l_readData = GetPTRMappedData();

	if (!l_readData)
	{
		DestroyMemoryMappedFile();
		FWK_ASSERT_RETURN_VALUE("読み込むためのメモリマップドデータの取得に失敗しました。", false)
	}

	// StaticModelAssetHeaderより小さいファイルは、Headerを安全に読み込めないため壊れたAssetとして扱う
	// ※注意 : 簡易的なバージョンチェックなのでint型をstd::uint32_tに変えても変更に気づけない
	if (GetREFMappedDataSize() < sizeof(StaticModelAssetHeader))
	{

#if defined(_DEBUG)
		const auto& l_debugLog = std::format("StaticModelAssetのファイルサイズがHeaderサイズよりも小さいため、FBXから再生成します。AssetFileSize : {}, HeaderSize : {}\n", GetREFMappedDataSize(), sizeof(StaticModelAssetHeader));

		OutputDebugStringA(l_debugLog.c_str());
#endif
		
		DestroyMemoryMappedFile();

		return false;
	}

	auto l_readOffset = k_initialReadOffset;

	StaticModelAssetHeader l_staticModelAssetHeader = {};

	ReadBinaryData(k_singleBinaryElementCount,
				   l_readData,
				   l_readOffset,
				   &l_staticModelAssetHeader);

	if (l_staticModelAssetHeader.m_assetTypeID != k_staticModelAssetTypeID)
	{
#if defined (_DEBUG)
		const auto& l_debugLog = std::format("StaticModelのAssetTypeIDが一致しないため、FBXから再生成します。AssetTypeID : {}, CurrentAssetTypeID : {}\n", l_staticModelAssetHeader.m_assetTypeID, k_staticModelAssetTypeID);

		OutputDebugStringA(l_debugLog.c_str());
#endif

		DestroyMemoryMappedFile();

		return false;
	}

	// Assetの保存形式が現在のバージョンと異なる場合は、
	// 古いAssetを使用せずFBXから再生成するためfalseを返す
	if (l_staticModelAssetHeader.m_version != k_staticModelAssetVersion)
	{
#if defined(_DEBUG)
		const auto& l_debugLog = std::format("StaticModelAssetのVersionが一致しないため、FBXから再生成します。 AssetVersion : {}, CurrentVersion : {}\n", l_staticModelAssetHeader.m_version, k_staticModelAssetVersion);

		OutputDebugStringA(l_debugLog.c_str());
#endif 
		DestroyMemoryMappedFile();

		return false;
	}

	if (l_staticModelAssetHeader.m_fileSize != GetREFMappedDataSize())
	{
#if defined (_DEBUG)
		const auto& l_debugLog = std::format("StaticModelのファイルサイズが一致しないため、FBXから再生成します。AssetFileSize : {}, CurrentFileSize : {}\n", l_staticModelAssetHeader.m_fileSize, GetREFMappedDataSize());

		OutputDebugStringA(l_debugLog.c_str());
#endif

		DestroyMemoryMappedFile();

		return false;
	}

	l_modelData.m_modelMeshList.clear  ();
	l_modelData.m_modelMeshList.reserve(l_staticModelAssetHeader.m_meshCount);

	for (std::uint64_t l_meshIndex = 0ULL; l_meshIndex < l_staticModelAssetHeader.m_meshCount; ++l_meshIndex)
	{
		StaticModelAssetMeshHeader l_staticModelAssetMeshHeader = {};

		// ヘッダー情報読み込み
		ReadBinaryData(k_singleBinaryElementCount,
					   l_readData,
					   l_readOffset,
					   &l_staticModelAssetMeshHeader);

		Struct::ModelMesh l_modelMesh = {};

		l_modelMesh.m_modelVertexList.resize(l_staticModelAssetMeshHeader.m_vertexCount);
		l_modelMesh.m_indexList.resize      (l_staticModelAssetMeshHeader.m_indexCount);

		// 頂点情報読み込み
		ReadBinaryData(l_staticModelAssetMeshHeader.m_vertexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMesh.m_modelVertexList.data());

		// インデックス情報読み込み
		ReadBinaryData(l_staticModelAssetMeshHeader.m_indexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMesh.m_indexList.data());

		auto& l_modelMeshletData = l_modelMesh.m_modelMeshletData;

		l_modelMeshletData.m_meshletList.resize		     (l_staticModelAssetMeshHeader.m_meshletCount);
		l_modelMeshletData.m_uniqueVertexIndexList.resize(l_staticModelAssetMeshHeader.m_uniqueVertexIndexCount);
		l_modelMeshletData.m_primitiveIndexList.resize   (l_staticModelAssetMeshHeader.m_primitiveIndexCount);
		l_modelMeshletData.m_meshletBoundsList.resize    (l_staticModelAssetMeshHeader.m_meshletBoundsCount);

		// メッシュレット情報読み込み
		ReadBinaryData(l_staticModelAssetMeshHeader.m_meshletCount,
					   l_readData,	
					   l_readOffset,
					   l_modelMeshletData.m_meshletList.data());

		// メッシュレット内ユニーク頂点インデックス情報読み込み
		ReadBinaryData(l_staticModelAssetMeshHeader.m_uniqueVertexIndexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMeshletData.m_uniqueVertexIndexList.data());

		// メッシュレット内プリミティブインデックス情報読み込み
		ReadBinaryData(l_staticModelAssetMeshHeader.m_primitiveIndexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMeshletData.m_primitiveIndexList.data());

		// メッシュレット境界情報読み込み
		ReadBinaryData(l_staticModelAssetMeshHeader.m_meshletBoundsCount,
					   l_readData,
					   l_readOffset,
					   l_modelMeshletData.m_meshletBoundsList.data());

		auto& l_modelMaterialAssetData = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;

		// テクスチャファイルパス情報読み込み
		// ベースカラーテクスチャファイルパス読み込み
		ReadWStringBinaryData(l_staticModelAssetMeshHeader.m_baseColorTexturePathSize, 
							  l_readData,
							  l_modelMaterialAssetData.m_baseColorTextureFilePath,
							  l_readOffset);

		// 法線テクスチャファイルパス読み込み
		ReadWStringBinaryData(l_staticModelAssetMeshHeader.m_normalTexturePathSize,
							  l_readData,
							  l_modelMaterialAssetData.m_normalTextureFilePath,
						      l_readOffset);

		l_modelData.m_modelMeshList.emplace_back(std::move(l_modelMesh));
	}

	if (l_readOffset != GetREFMappedDataSize())
	{
#if defined(_DEBUG)
		const auto& l_debugLog = std::format("StaticModelAssetの読み込みサイズがファイルサイズと一致しないため、FBXから再生成します。ReadSize : {}, Filesize : {}\n", l_readOffset, GetREFMappedDataSize());

		OutputDebugStringA(l_debugLog.c_str());
#endif

		DestroyMemoryMappedFile();

		return false;
	}

	DestroyMemoryMappedFile();

	return true;
}
bool FWK::Converter::StaticModelBinaryConverter::SaveStaticModelAsset(const Graphics::StaticModelRecord& a_staticModelRecord, const std::filesystem::path& a_filePath)
{
	const auto& l_modelData = a_staticModelRecord.GetREFModelData();

	const auto& l_fileSize = CalculateStaticModelAssetFileSize(l_modelData);

	FWK_ASSERT_RETURN_VALUE_IF(l_fileSize == k_emptyStaticModelAssetFileSize,		 "StaticModelAssetのファイルサイズ計算に失敗しました。",     false)
	FWK_ASSERT_RETURN_VALUE_IF(!CreateWriteMemoryMappedFile(a_filePath, l_fileSize), "書き込むためのメモリマップドファイル作成に失敗しました。", false)
	
	auto* l_writeData = GetMutablePTRMappedData();

	if (!l_writeData)
	{
		DestroyMemoryMappedFile();
		FWK_ASSERT_RETURN_VALUE("書き込むためのメモリマップドデータの取得に失敗しました。", false)
	}

	auto l_writeOffset = k_initialWriteOffset;

	StaticModelAssetHeader l_staticModelAssetHeader = {};

	l_staticModelAssetHeader.m_fileSize    = l_fileSize;
	l_staticModelAssetHeader.m_meshCount   = l_modelData.m_modelMeshList.size();
	l_staticModelAssetHeader.m_assetTypeID = k_staticModelAssetTypeID;
	l_staticModelAssetHeader.m_version     = k_staticModelAssetVersion;

	WriteBinaryData(k_singleBinaryElementCount,
					&l_staticModelAssetHeader,
					l_writeOffset,
					l_writeData);

	for (const auto& l_modelMesh : l_modelData.m_modelMeshList)
	{
		const auto& l_modelMaterialAssetData = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;
		const auto& l_modelMeshletData       = l_modelMesh.m_modelMeshletData;

		StaticModelAssetMeshHeader l_staticModelAssetMeshHeader = {};

		l_staticModelAssetMeshHeader.m_vertexCount              = l_modelMesh.m_modelVertexList.size             ();
		l_staticModelAssetMeshHeader.m_indexCount               = l_modelMesh.m_indexList.size                   ();
		l_staticModelAssetMeshHeader.m_meshletCount             = l_modelMeshletData.m_meshletList.size          ();
		l_staticModelAssetMeshHeader.m_uniqueVertexIndexCount   = l_modelMeshletData.m_uniqueVertexIndexList.size();
		l_staticModelAssetMeshHeader.m_primitiveIndexCount		= l_modelMeshletData.m_primitiveIndexList.size   ();
		l_staticModelAssetMeshHeader.m_meshletBoundsCount		= l_modelMeshletData.m_meshletBoundsList.size    ();
		l_staticModelAssetMeshHeader.m_baseColorTexturePathSize = CalculateWStringBinaryFileSize                 (l_modelMaterialAssetData.m_baseColorTextureFilePath);
		l_staticModelAssetMeshHeader.m_normalTexturePathSize    = CalculateWStringBinaryFileSize				 (l_modelMaterialAssetData.m_normalTextureFilePath);

		// ヘッダー情報保存
		WriteBinaryData(k_singleBinaryElementCount,
					    &l_staticModelAssetMeshHeader,
					    l_writeOffset,
						l_writeData);

		// 頂点情報保存
		WriteBinaryData(l_modelMesh.m_modelVertexList.size(),
						l_modelMesh.m_modelVertexList.data(),
						l_writeOffset,
						l_writeData);

		// インデックス情報保存
		WriteBinaryData(l_modelMesh.m_indexList.size(),
						l_modelMesh.m_indexList.data(),
						l_writeOffset,
						l_writeData);

		// メッシュレット情報保存
		WriteBinaryData(l_modelMeshletData.m_meshletList.size(),
						l_modelMeshletData.m_meshletList.data(), 
					    l_writeOffset,
						l_writeData);

		// メッシュレット内ユニーク頂点インデックス情報保存
		WriteBinaryData(l_modelMeshletData.m_uniqueVertexIndexList.size(),
						l_modelMeshletData.m_uniqueVertexIndexList.data(),
					    l_writeOffset,
						l_writeData);

		// メッシュレット内プリミティブインデックス情報保存
		WriteBinaryData(l_modelMeshletData.m_primitiveIndexList.size(),
						l_modelMeshletData.m_primitiveIndexList.data(),
					    l_writeOffset,
						l_writeData);

		// メッシュレット境界情報保存
		WriteBinaryData(l_modelMeshletData.m_meshletBoundsList.size(),
						l_modelMeshletData.m_meshletBoundsList.data(),
					    l_writeOffset,
						l_writeData);

		// テクスチャファイルパス情報保存
		// ベースカラーテクスチャファイルパス保存
		WriteWStringBinaryData(l_modelMaterialAssetData.m_baseColorTextureFilePath, l_writeOffset, l_writeData);
		WriteWStringBinaryData(l_modelMaterialAssetData.m_normalTextureFilePath,    l_writeOffset, l_writeData);
	}

	if (l_writeOffset != l_fileSize)
	{
		DestroyMemoryMappedFile();
		FWK_ASSERT_RETURN_VALUE("StaticModelAssetの書き込みサイズが計算サイズと一致しません。", false)
	}

	DestroyMemoryMappedFile();

	return true;
}

std::uint64_t FWK::Converter::StaticModelBinaryConverter::CalculateStaticModelAssetFileSize(const Struct::ModelData& a_modelData) const
{
	auto l_fileSize = CalculateBinaryDataSize<StaticModelAssetHeader>(k_singleBinaryElementCount);

	for (const auto& l_modelMesh : a_modelData.m_modelMeshList)
	{
		const auto& l_modelMaterialAssetData = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;

		l_fileSize += CalculateBinaryDataSize<StaticModelAssetMeshHeader>(k_singleBinaryElementCount);
		l_fileSize += CalculateBinaryDataSize<Struct::ModelVertex>		 (l_modelMesh.m_modelVertexList.size());
		l_fileSize += CalculateBinaryDataSize<std::uint32_t>			 (l_modelMesh.m_indexList.size());

		const auto& l_modelMeshletData = l_modelMesh.m_modelMeshletData;

		l_fileSize += CalculateBinaryDataSize<Struct::ModelMeshlet>      (l_modelMeshletData.m_meshletList.size());
		l_fileSize += CalculateBinaryDataSize<std::uint32_t>	         (l_modelMeshletData.m_uniqueVertexIndexList.size());
		l_fileSize += CalculateBinaryDataSize<std::uint32_t>	         (l_modelMeshletData.m_primitiveIndexList.size());
		l_fileSize += CalculateBinaryDataSize<Struct::ModelMeshletBounds>(l_modelMeshletData.m_meshletBoundsList.size());

		l_fileSize += CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_baseColorTextureFilePath);
		l_fileSize += CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_normalTextureFilePath);
	}

	return l_fileSize;
}