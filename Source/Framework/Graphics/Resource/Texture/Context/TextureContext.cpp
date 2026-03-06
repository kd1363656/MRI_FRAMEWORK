#include "TextureContext.h"

void FWK::Graphics::TextureContext::Init()
{
	m_nextID = k_initialID;

	m_pathToIDMap.clear           ();
	m_recordList.clear            ();
	m_textureIDToSRVIndexMap.clear();
	
	m_textureUploader.Init();

	const TextureLoaderFunc& l_wicFileLoader = [](const std::wstring& a_filePath, DirectX::TexMetadata& a_texMetadata, DirectX::ScratchImage& a_scratchImage) 
	{
		return DirectX::LoadFromWICFile(a_filePath.c_str(),
										DirectX::WIC_FLAGS_FORCE_RGB,
										&a_texMetadata,
										a_scratchImage);
	};

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionPNG,  l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionJPG,  l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionJPEG, l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionBMP,  l_wicFileLoader);

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionDDS, [](const std::wstring& a_filePath, DirectX::TexMetadata& a_texMetadata, DirectX::ScratchImage& a_scratchImage)
	{
			return DirectX::LoadFromDDSFile(a_filePath.c_str(),
											DirectX::DDS_FLAGS_NONE,
											&a_texMetadata,
											a_scratchImage);
	});

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionTGA, [](const std::wstring& a_filePath, DirectX::TexMetadata& a_texMetadata, DirectX::ScratchImage& a_scratchImage)
	{
			return DirectX::LoadFromTGAFile(a_filePath.c_str(),
											&a_texMetadata,
											a_scratchImage);
	});
}

FWK::Graphics::Texture FWK::Graphics::TextureContext::LoadFile(const std::string& a_filePath)
{
	// 既にロード済みならキャッシュを返す
	if (auto l_itr = m_pathToIDMap.find(a_filePath);
		l_itr != m_pathToIDMap.end())
	{
		return Texture(l_itr->second);
	}

	// 拡張子取得
	auto l_extension = std::filesystem::path(a_filePath).extension().string();

	// すべての文字を小文字に変換
	std::transform(l_extension.begin(),
				   l_extension.end  (),
				   l_extension.begin(),
				   [](unsigned char a_char) 
					{
						return static_cast<char>(std::tolower(a_char));
					});

	// 拡張子から最適なローダーを取得(LoadFromWICFileやLoadFromDDSFile)
	auto l_loaderItr = m_textureLoaderMap.find(l_extension);

	if (l_loaderItr == m_textureLoaderMap.end())
	{
		assert(false && "未対応のテクスチャフォーマットです。");
		return Texture();
	}

	// ワイド文字列に変換
	const auto& l_filePath = Utility::String::StringToWString(a_filePath);

	auto l_texMetadata  = DirectX::TexMetadata ();
	auto l_scratchImage = DirectX::ScratchImage();

	// 取得したローダーを使用してテクスチャを情報に変換
	const auto l_hr = l_loaderItr->second(l_filePath, l_texMetadata, l_scratchImage);

	if (FAILED(l_hr))
	{
		assert(false && "テクスチャデコード失敗。");
		return Texture();
	}

	auto l_record = CommonStruct::TextureRecord();

	const TextureID l_currentTextureID = m_nextTextureID;

	// ファイルパスとテクスチャIDを格納
	l_record.textureID  = l_currentTextureID;
	l_record.sourcePath = a_filePath;

	if (!m_textureUploader.StageTexture(l_texMetadata, l_scratchImage, l_record))
	{
		assert(false && "StageTexture失敗。");
		return Texture();
	}

	m_pathToIDMap[a_filePath] = l_currentTextureID;

	// 次回のテクスチャID用にインクリメント
	++m_nextTextureID;

	return Texture(l_currentTextureID);
}

void FWK::Graphics::TextureContext::FlushUploads()
{
	std::vector<CommonStruct::TextureRecord> l_uploadedList;

	m_textureUploader.FlushUploads(l_uploadedList);

	for (auto& l_record : l_uploadedList)
	{
		m_textureIDToSRVIndexMap[l_record.textureID] = l_record.srvIndex;

		m_recordList.emplace_back(std::move(l_record));
	}
}

UINT FWK::Graphics::TextureContext::FetchSRVIndex(const TextureID a_id) const
{
	// TextureID -> SRVIndexの高速検索
	if (auto l_itr = m_textureIDToSRVIndexMap.find(a_id);
		l_itr != m_textureIDToSRVIndexMap.end())
	{
		return l_itr->second;
	}

	assert(false && "指定テクスチャIDに対応するSRVインデックスが見つかりませんが見つかりません。");
	return k_invalideSRVIndex;
}