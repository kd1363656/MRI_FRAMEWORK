#include "StaticModelBinaryConverter.h"

bool FWK::Converter::StaticModelBinaryConverter::LoadStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath)
{
	const auto& l_staticModelRecord = a_staticModelRecord.lock();

	if (!l_staticModelRecord)
	{
		assert(false && "StaticModelRecordが無効のため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	auto& l_modelData = l_staticModelRecord->m_modelData;

	if (!CreateReadMemoryMappedFile(a_filePath))
	{
		assert(false && "読み込むためのメモリマップドファイル作成に失敗しました。");
		return false;
	}

	const auto* l_readData = GetPTRMappedData();

	if (!l_readData)
	{
		assert                 (false && "読み込むためのメモリマップドデータの取得に失敗しました。");
		DestroyMemoryMappedFile();

		return false;
	}

	// StaticModelAssetHeaderより小さいファイルは、Headerを安全に読み込めないため壊れたAssetとして扱う
	// ※注意 : 簡易的なバージョンチェックなのでint型をstd::uint32_tに変えても変更に気づけない
	if (GetREFMappedDataSize() < sizeof(StaticModelAssetHeader))
	{
		assert(false && "StaticModelAssetのファイルサイズがHeaderサイズより小さいです。");
		return false;
	}

	
	auto l_readOffset = k_initialReadOffset;

	StaticModelAssetHeader l_staticModelAssetHeader = {};

	ReadBinaryData(sizeof(StaticModelAssetHeader),
				   l_readData,
				   l_readOffset,
				   &l_staticModelAssetHeader);

	if (l_staticModelAssetHeader.m_assetTypeID != k_staticModelAssetTypeID)
	{
		assert				   (false && "StaticModelAssetのAssetTypeIDが一致しません。");
		DestroyMemoryMappedFile();

		return false;
	}

	// Assetの保存形式が現在のバージョンと異なる場合は、
	// 古いAssetを使用せずFBXから再生成するためfalseを返す
	if (l_staticModelAssetHeader.m_version != k_staticModelAssetVersion)
	{
#if defined(_DEBUG)
		const auto l_debugLog = std::format("StaticModelAssetのVersionが一致しないため、FBXから再生成します。 AssetVersion : {}, CurrentVersion : {}\n", 
											 l_staticModelAssetHeader.m_version,
											 k_staticModelAssetVersion);

		OutputDebugStringA(l_debugLog.c_str());
#endif 
		DestroyMemoryMappedFile();

		return false;
	}

	if (l_staticModelAssetHeader.m_fileSize != GetREFMappedDataSize())
	{
		assert				   (false && "StaticModelAssetのファイルサイズが一致しません。");
		DestroyMemoryMappedFile();

		return false;
	}

	l_modelData.m_modelMeshList.clear  ();
	l_modelData.m_modelMeshList.reserve(l_staticModelAssetHeader.m_meshCount);

	for (std::uint64_t l_meshIndex = 0ULL; l_meshIndex < l_staticModelAssetHeader.m_meshCount; ++l_meshIndex)
	{
		StaticModelAssetMeshHeader l_staticModelAssetMeshHeader = {};

		// ヘッダー情報読み込み
		ReadBinaryData(sizeof(StaticModelAssetMeshHeader),
					   l_readData,
					   l_readOffset,
					   &l_staticModelAssetMeshHeader);

		Struct::ModelMesh l_modelMesh = {};

		l_modelMesh.m_modelVertexList.resize(l_staticModelAssetMeshHeader.m_vertexCount);
		l_modelMesh.m_indexList.resize      (l_staticModelAssetMeshHeader.m_indexCount);

		// 頂点情報読み込み
		ReadBinaryData(sizeof(Struct::ModelVertex) * l_staticModelAssetMeshHeader.m_vertexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMesh.m_modelVertexList.data());

		// インデックス情報読み込み
		ReadBinaryData(sizeof(std::uint32_t) * l_staticModelAssetMeshHeader.m_indexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMesh.m_indexList.data());

		auto& l_modelMeshletData = l_modelMesh.m_modelMeshletData;

		l_modelMeshletData.m_meshletList.resize		     (l_staticModelAssetMeshHeader.m_meshletCount);
		l_modelMeshletData.m_uniqueVertexIndexList.resize(l_staticModelAssetMeshHeader.m_uniqueVertexIndexCount);
		l_modelMeshletData.m_primitiveIndexList.resize   (l_staticModelAssetMeshHeader.m_primitiveIndexCount);
		l_modelMeshletData.m_meshletBoundsList.resize    (l_staticModelAssetMeshHeader.m_meshletBoundsCount);

		// メッシュレット情報読み込み
		ReadBinaryData(sizeof(Struct::ModelMeshlet) * l_staticModelAssetMeshHeader.m_meshletBoundsCount,
					   l_readData,	
					   l_readOffset,
					   l_modelMeshletData.m_meshletList.data());

		// メッシュレット内ユニーク頂点インデックス情報読み込み
		ReadBinaryData(sizeof(std::uint32_t) * l_staticModelAssetMeshHeader.m_uniqueVertexIndexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMeshletData.m_uniqueVertexIndexList.data());

		// メッシュレット内プリミティブインデックス情報読み込み
		ReadBinaryData(sizeof(std::uint8_t) * l_staticModelAssetMeshHeader.m_primitiveIndexCount,
					   l_readData,
					   l_readOffset,
					   l_modelMeshletData.m_primitiveIndexList.data());

		// メッシュレット境界情報読み込み
		ReadBinaryData(sizeof(Struct::ModelMeshletBounds) * l_staticModelAssetMeshHeader.m_meshletBoundsCount,
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

		// ラフネステクスチャファイルパス読み込み
		ReadWStringBinaryData(l_staticModelAssetMeshHeader.m_roughnessTexturePathSize,
							  l_readData,
							  l_modelMaterialAssetData.m_roughnessTextureFilePath,
							  l_readOffset);

		// メタリックテクスチャファイルパス読み込み
		ReadWStringBinaryData(l_staticModelAssetMeshHeader.m_metallicTexturePathSize,
							  l_readData,
							  l_modelMaterialAssetData.m_metallicTextureFilePath,
							  l_readOffset);

		l_modelData.m_modelMeshList.emplace_back(std::move(l_modelMesh));
	}

	if (l_readOffset != GetREFMappedDataSize())
	{
		assert				   (false && "StaticModelAssetの読み込みサイズがファイルサイズと一致しません。");
		DestroyMemoryMappedFile();

		return false;
	}

	DestroyMemoryMappedFile();

	return true;
}
bool FWK::Converter::StaticModelBinaryConverter::SaveStaticModelAsset(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, const std::filesystem::path& a_filePath)
{
	const auto& l_staticModelRecord = a_staticModelRecord.lock();

	if (!l_staticModelRecord)
	{
		assert(false && "StaticModelRecordが無効のため、StaticModelの読み込みに失敗しました。");
		return false;
	}

	const auto& l_modelData = l_staticModelRecord->m_modelData;

	const auto& l_fileSize = CalculateStaticModelAssetFileSize(l_modelData);

	if (l_fileSize == k_emptyStaticModelAssetFileSize)
	{
		assert(false && "StaticModelAssetのファイルサイズ計算に失敗しました。");
		return false;
	}

	if (!CreateWriteMemoryMappedFile(a_filePath, l_fileSize)) 
	{
		assert(false && "書き込むためのメモリマップドファイル作成に失敗しました。");
		return false; 
	}

	auto* l_writeData = GetMutablePTRMappedData();

	if (!l_writeData)
	{
		assert				   (false && "書き込むためのメモリマップドデータの取得に失敗しました。");
		DestroyMemoryMappedFile();

		return false;
	}

	auto l_writeOffset = k_initialWriteOffset;

	StaticModelAssetHeader l_staticModelAssetHeader = {};

	l_staticModelAssetHeader.m_fileSize    = l_fileSize;
	l_staticModelAssetHeader.m_meshCount   = l_modelData.m_modelMeshList.size();
	l_staticModelAssetHeader.m_assetTypeID = k_staticModelAssetTypeID;
	l_staticModelAssetHeader.m_version     = k_staticModelAssetVersion;

	WriteBinaryData(sizeof(StaticModelAssetHeader),
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
		l_staticModelAssetMeshHeader.m_normalTexturePathSize    = CalculateWStringBinaryFileSize                 (l_modelMaterialAssetData.m_normalTextureFilePath);
		l_staticModelAssetMeshHeader.m_roughnessTexturePathSize = CalculateWStringBinaryFileSize                 (l_modelMaterialAssetData.m_roughnessTextureFilePath);
		l_staticModelAssetMeshHeader.m_metallicTexturePathSize  = CalculateWStringBinaryFileSize                 (l_modelMaterialAssetData.m_metallicTextureFilePath);
																								       
		// ヘッダー情報保存
		WriteBinaryData(sizeof(StaticModelAssetMeshHeader),
					    &l_staticModelAssetMeshHeader,
					    l_writeOffset,
						l_writeData);

		// 頂点情報保存
		WriteBinaryData(sizeof(Struct::ModelVertex) * l_modelMesh.m_modelVertexList.size(),
						l_modelMesh.m_modelVertexList.data(),
						l_writeOffset,
						l_writeData);

		// インデックス情報保存
		WriteBinaryData(sizeof(std::uint32_t) * l_modelMesh.m_indexList.size(),
						l_modelMesh.m_indexList.data(),
						l_writeOffset,
						l_writeData);

		// メッシュレット情報保存
		WriteBinaryData(sizeof(Struct::ModelMeshlet) * l_modelMeshletData.m_meshletList.size(),
						l_modelMeshletData.m_meshletList.data(), 
					    l_writeOffset,
						l_writeData);

		// メッシュレット内ユニーク頂点インデックス情報保存
		WriteBinaryData(sizeof(std::uint32_t) * l_modelMeshletData.m_uniqueVertexIndexList.size(),
						l_modelMeshletData.m_uniqueVertexIndexList.data(),
					    l_writeOffset,
						l_writeData);

		// メッシュレット内プリミティブインデックス情報保存
		WriteBinaryData(sizeof(std::uint8_t) * l_modelMeshletData.m_primitiveIndexList.size(),
						l_modelMeshletData.m_primitiveIndexList.data(),
					    l_writeOffset,
						l_writeData);

		// メッシュレット境界情報保存
		WriteBinaryData(sizeof(Struct::ModelMeshletBounds) * l_modelMeshletData.m_meshletBoundsList.size(),
						l_modelMeshletData.m_meshletBoundsList.data(),
					    l_writeOffset,
						l_writeData);

		// テクスチャファイルパス情報保存
		// ベースカラーテクスチャファイルパス保存
		WriteWStringBinaryData(l_modelMaterialAssetData.m_baseColorTextureFilePath, l_writeOffset, l_writeData);

		// 法線テクスチャファイルパス保存
		WriteWStringBinaryData(l_modelMaterialAssetData.m_normalTextureFilePath, l_writeOffset, l_writeData);

		// ラフネステクスチャファイルパス保存
		WriteWStringBinaryData(l_modelMaterialAssetData.m_roughnessTextureFilePath, l_writeOffset, l_writeData);

		// メタリックテクスチャファイルパス保存
		WriteWStringBinaryData(l_modelMaterialAssetData.m_metallicTextureFilePath, l_writeOffset, l_writeData);
	}

	if (l_writeOffset != l_fileSize)
	{
		assert				   (false && "StaticModelAssetの書き込みサイズが計算サイズと一致しません。");
		DestroyMemoryMappedFile();

		return false;
	}

	DestroyMemoryMappedFile();

	return true;
}

void FWK::Converter::StaticModelBinaryConverter::ReadBinaryData(const std::uint64_t& a_readDataSize,
																const std::uint8_t*  a_readData,
																	  std::uint64_t& a_readOffset,
																	  void*			 a_destinationData) const
{
	if (a_readDataSize == k_emptyReadDataSize) { return; }

	if (!a_destinationData)
	{
		assert(false && "読み込み先データがnullptrです。");
		return;
	}

	if (!a_readData)
	{
		assert(false && "読み込み元データがnullptrです。");
		return;
	}

	std::memcpy(a_destinationData, a_readData + a_readOffset, a_readDataSize);

	a_readOffset += a_readDataSize;
}
void FWK::Converter::StaticModelBinaryConverter::ReadWStringBinaryData(const std::uint64_t& a_stringBinaryFileSize, 
																	   const std::uint8_t*  a_readData,
																		     std::wstring&  a_string, 
																			 std::uint64_t& a_readOffset) const
{
	if (a_stringBinaryFileSize == k_emptyReadDataSize)
	{
		a_string.clear();
		return;
	}

	const auto& l_stringLength = a_stringBinaryFileSize / sizeof(wchar_t);

	a_string.resize(l_stringLength);

	ReadBinaryData(a_stringBinaryFileSize,
				   a_readData, 
				   a_readOffset,
				   a_string.data());
}

void FWK::Converter::StaticModelBinaryConverter::WriteBinaryData(const std::uint64_t& a_writeDataSize, 
																 const void*		  a_sourceData,
																	   std::uint64_t& a_writeOffset,
																	   std::uint8_t*  a_writeData) const
{
	if (a_writeDataSize == k_emptyWriteDataSize) { return; }

	if (!a_sourceData)
	{
		assert(false && "書き込み元データがnullptrです。");
		return;
	}

	if (!a_writeData)
	{
		assert(false && "書き込み先データがnullptrです。");
		return;
	}

	std::memcpy(a_writeData + a_writeOffset, a_sourceData, a_writeDataSize);

	a_writeOffset += a_writeDataSize;
}
void FWK::Converter::StaticModelBinaryConverter::WriteWStringBinaryData(const std::wstring& a_string, std::uint64_t& a_writeOffset, std::uint8_t* a_writeData) const
{
	const auto& l_stringBinaryFileSize = CalculateWStringBinaryFileSize(a_string);

	if (l_stringBinaryFileSize == k_emptyWriteDataSize) { return; }

	WriteBinaryData(l_stringBinaryFileSize, a_string.data(), a_writeOffset, a_writeData);
}

std::uint64_t FWK::Converter::StaticModelBinaryConverter::CalculateStaticModelAssetFileSize(const Struct::ModelData& a_modelData) const
{
	auto l_fileSize = sizeof(StaticModelAssetHeader);

	for (const auto& l_modelMesh : a_modelData.m_modelMeshList)
	{
		const auto& l_modelMaterialAssetData = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;

		l_fileSize += sizeof(StaticModelAssetMeshHeader);
		l_fileSize += sizeof(Struct::ModelVertex) * l_modelMesh.m_modelVertexList.size();
		l_fileSize += sizeof(std::uint32_t)       * l_modelMesh.m_indexList.size      ();

		const auto& l_modelMeshletData = l_modelMesh.m_modelMeshletData;

		l_fileSize += sizeof(Struct::ModelMeshlet)       * l_modelMeshletData.m_meshletList.size          ();
		l_fileSize += sizeof(std::uint32_t)              * l_modelMeshletData.m_uniqueVertexIndexList.size();
		l_fileSize += sizeof(std::uint8_t)               * l_modelMeshletData.m_primitiveIndexList.size   ();
		l_fileSize += sizeof(Struct::ModelMeshletBounds) * l_modelMeshletData.m_meshletBoundsList.size    ();

		l_fileSize += CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_baseColorTextureFilePath);
		l_fileSize += CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_normalTextureFilePath);
		l_fileSize += CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_roughnessTextureFilePath);
		l_fileSize += CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_metallicTextureFilePath);
	}

	return l_fileSize;
}
std::uint64_t FWK::Converter::StaticModelBinaryConverter::CalculateWStringBinaryFileSize(const std::wstring& a_string) const
{
	return sizeof(wchar_t) * a_string.size();
}