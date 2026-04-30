#include "TextureSystem.h"

FWK::TypeAlias::TextureID FWK::Graphics::TextureSystem::RegisterTexture(const Device&			                 a_device,
																	    const GPUMemoryAllocator&                a_gpuMemoryAllocator,
																		const std::filesystem::path&             a_filePath, 
																			  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorHeap,
																			  UploadSystem&						 a_uploadSystem)
{
	if (a_filePath.empty())
	{
		assert(false && "登録対象のテクスチャパスが空のため、テクスチャ登録に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	// "."や".."などの表記ゆれを整理した、TextureSystem内部の管理用ファイルパスを取得
	const auto& l_normalizedTextureFilePath = Utility::File::MakeNormalizedFilePath(a_filePath);
	
	// 取得したファイルパスの拡張子が".dds"のようにすべて小文字でない場合return
	if (l_normalizedTextureFilePath.extension() != Constant::k_lowerDDSExtension)
	{
		assert(false && "拡張子が.ddsでないため、テクスチャ登録に失敗しました、.DDSのように大文字が含まれている可能性があります。");
		return Constant::k_invalidTextureID;
	}

	// テクスチャファイルパスをwstringに変換
	const auto& l_textureFilePath = l_normalizedTextureFilePath.wstring();

	// 変換したファイルパスから一致するキーを持つならreturn(新規でテクスチャIDを発行する必要がないため)
	if (const auto& l_itr = m_texturePathMap.find(l_textureFilePath);
		l_itr != m_texturePathMap.end())
	{
		return l_itr->second;
	}

	DirectX::ScratchImage l_scratchImage = {};
	DirectX::TexMetadata  l_texMetadata  = {};

	// テクスチャファイルを読み込みl_scratchImageとl_texMetadataにデータを格納
	if (!m_textureLoader.LoadTextureFile(l_scratchImage,
										 l_texMetadata,
										 l_textureFilePath))
	{
		assert(false && "テクスチャ読み込みに失敗したため、テクスチャ登録に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	// テクスチャIDを発行
	const auto l_textureID = GenerateTextureID();

	if (l_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDの採番に失敗したため、テクスチャ登録に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	Struct::TextureRecord l_textureRecord = {};

	const auto l_srvIndex = a_srvDescriptorHeap.Allocate();

	if (l_srvIndex == Constant::k_invalidDescriptorHeapIndex)
	{
		assert(false && "SRVディスクリプタヒープインデックスの確保に失敗したため、テクスチャ登録に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	l_textureRecord.m_srvIndex  = l_srvIndex;
	l_textureRecord.m_textureID = l_textureID;
	l_textureRecord.m_filePath  = l_textureFilePath;

	// 読み込んだDDSの画像データをTextureResourceへアップロードし、SRVを作成する
	if (!m_textureUploader.UploadTexture(l_scratchImage,
										 l_texMetadata,
										 a_device,
										 a_gpuMemoryAllocator,
										 a_srvDescriptorHeap,
										 a_uploadSystem,
										 l_textureRecord))
	{
		assert(false && "テクスチャアップロードに失敗したため、テクスチャ登録に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	m_texturePathMap.try_emplace  (l_textureFilePath, l_textureID);
	m_textureRecordMap.try_emplace(l_textureID,       std::move(l_textureRecord));

	return l_textureID;
}

FWK::TypeAlias::TextureID FWK::Graphics::TextureSystem::GenerateTextureID()
{
	// テクスチャIDを新規用に更新
	++m_nextTextureID;

	if (m_nextTextureID == Constant::k_invalidTextureID)
	{
		assert(false && "TextureIDが上限に到達したため、TextureIDの採番に失敗しました。");
		return Constant::k_invalidTextureID;
	}

	return m_nextTextureID;
}