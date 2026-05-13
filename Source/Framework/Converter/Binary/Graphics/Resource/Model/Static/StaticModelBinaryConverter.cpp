#include "StaticModelBinaryConverter.h"

bool FWK::Converter::StaticModelBinaryConverter::LoadStaticModelAsset(Struct::ModelData& a_modelData, const std::filesystem::path& a_filePath)
{
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

	if (l_staticModelAssetHeader.m_version != k_staticModelAssetVersion)
	{
		assert				   (false && "StaticModelAssetのVersionが一致しません。");
		DestroyMemoryMappedFile();

		return false;
	}

	if (l_staticModelAssetHeader.m_fileSize != GetREFMappedDataSize())
	{
		assert				   (false && "StaticModelAssetのファイルサイズが一致しません。");
		DestroyMemoryMappedFile();

		return false;
	}

	a_modelData.m_modelMeshList.clear  ();
	a_modelData.m_modelMeshList.reserve(l_staticModelAssetHeader.m_meshCount);

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

		a_modelData.m_modelMeshList.emplace_back(std::move(l_modelMesh));
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

bool FWK::Converter::StaticModelBinaryConverter::SaveStaticModelAsset(const Struct::ModelData& a_modelData, const std::filesystem::path& a_filePath)
{
	const auto& l_fileSize = CalculateStaticModelAssetFileSize(a_modelData);

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

	l_staticModelAssetHeader.m_fileSize  = l_fileSize;
	l_staticModelAssetHeader.m_meshCount = a_modelData.m_modelMeshList.size();

	WriteBinaryData(sizeof(StaticModelAssetHeader),
					&l_staticModelAssetHeader,
					l_writeOffset,
					l_writeData);

	for (const auto& l_modelMesh : a_modelData.m_modelMeshList)
	{
		const auto& l_modelMaterialAssetData = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;

		StaticModelAssetMeshHeader l_staticModelAssetMeshHeader = {};

		l_staticModelAssetMeshHeader.m_vertexCount              = l_modelMesh.m_modelVertexList.size();
		l_staticModelAssetMeshHeader.m_indexCount               = l_modelMesh.m_indexList.size       ();
		l_staticModelAssetMeshHeader.m_baseColorTexturePathSize = CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_baseColorTextureFilePath);
		l_staticModelAssetMeshHeader.m_normalTexturePathSize    = CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_normalTextureFilePath);
		l_staticModelAssetMeshHeader.m_roughnessTexturePathSize = CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_roughnessTextureFilePath);
		l_staticModelAssetMeshHeader.m_metallicTexturePathSize  = CalculateWStringBinaryFileSize(l_modelMaterialAssetData.m_metallicTextureFilePath);

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