#include "TextureContext.h"

void FWK::Graphics::TextureContext::Init()
{
	m_nextID = k_initialID;

	m_pathToIDMap.clear();
	m_recordList.clear ();

	const auto& l_wicFileLoader = [](const std::wstring& a_filePath, DirectX::TexMetadata* a_texMetadata, DirectX::ScratchImage& a_scratchImage) 
	{
		return DirectX::LoadFromWICFile(a_filePath.c_str(),
										DirectX::WIC_FLAGS_FORCE_RGB,
										a_texMetadata,
										a_scratchImage);
	};

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionPNG,  l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionJPG,  l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionJPEG, l_wicFileLoader);
	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionBMP,  l_wicFileLoader);

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionDDS, [](const std::wstring& a_filePath, DirectX::TexMetadata* a_texMetadata, DirectX::ScratchImage& a_scratchImage)
	{
			return DirectX::LoadFromDDSFile(a_filePath.c_str(),
											DirectX::DDS_FLAGS_NONE,
											a_texMetadata,
											a_scratchImage);
	});

	m_textureLoaderMap.try_emplace(CommonConstant::k_extensionTGA, [](const std::wstring& a_filePath, DirectX::TexMetadata* a_texMetadata, DirectX::ScratchImage& a_scratchImage)
	{
			return DirectX::LoadFromTGAFile(a_filePath.c_str(),
											a_texMetadata,
											a_scratchImage);
	});
}

bool FWK::Graphics::TextureContext::Create()
{
	// ※将来: UploadHeapAllocatorやTextureHeapAllocatorの初期化などをここに入れる
	return true;
}

FWK::Graphics::Texture FWK::Graphics::TextureContext::LoadFile(const std::string& a_filePath)
{
	// 拡張子を取得
	const auto l_extension = std::filesystem::path(a_filePath).extension().string();

	auto l_itr = m_textureLoaderMap.find(l_extension);

	if (l_itr == m_textureLoaderMap.end())
	{
		assert(false && "未対応のテクスチャフォーマットです。");
		return Texture();
	}

	return LoadTexture(a_filePath, l_itr->second);
}

UINT FWK::Graphics::TextureContext::FetchSRVIndex(const TextureID a_id) const
{
	return 0;
}

FWK::Graphics::Texture FWK::Graphics::TextureContext::LoadTexture(const std::string& a_filePath, const TextureLoaderFunc& a_textureLoaderFunc)
{
	// キャッシュ確認
	if (auto l_itr = m_pathToIDMap.find(a_filePath);
		l_itr != m_pathToIDMap.end())
	{
		return Texture(l_itr->second);
	}

	// DirectXTexで読み込み
	auto l_texMetadata  = DirectX::TexMetadata ();
	auto l_scratchImage = DirectX::ScratchImage();

	// ワイド文字列に変換
	const auto l_filePath = Utility::String::StringToWString(a_filePath);

	const auto l_hr = a_textureLoaderFunc(l_filePath, &l_texMetadata, &l_scratchImage);

	if (FAILED(l_hr))
	{
		assert(false && "DirectXTexでの画像読み込みに失敗しました。パスや対応形式を確認してください");
		return Texture();
	}

	// GPUリソース作成 & 転送
	auto l_record = CommonStruct::TextureRecord();

	l_record.textureID  = m_nextID++;
	l_record.sourcePath = a_filePath;

	if (!CreateTextureResourceAndUpload(a_filePath, l_texMetadata, l_scratchImage, l_record))
	{
		assert(false && "テクスチャのGPUアップロードに失敗しました。");
		return Texture();
	}

	// 登録
	m_pathToIDMap[a_filePath] = l_record.textureID;

	m_recordList.emplace_back(l_record);

	return Texture(l_record.textureID);
}

bool FWK::Graphics::TextureContext::CreateTextureResourceAndUpload(const std::string&           a_filePath, 
																   const DirectX::TexMetadata&  a_metadata,
																   const DirectX::ScratchImage& a_image,
																   CommonStruct::TextureRecord& a_outRecord)
{
	auto& l_graphicsManager = GraphicsManager::GetInstance();

	const auto& l_deviceWrapper = l_graphicsManager.GetHardware().GetDevice();
	const auto& l_device        = l_deviceWrapper.GetDevice    ();

	auto& l_resourceContext = l_graphicsManager.GetResourceContext();

	const auto& l_copyCommandQueue     = l_resourceContext.GetCopyCommandQueue    ();
	const auto& l_copyCommandAllocator = l_resourceContext.GetCopyCommandAllocator();
	const auto& l_copyCommandList      = l_resourceContext.GetCopyCommandList     ();

	auto&       l_srvDescriptorAllocator = l_resourceContext.GetWorkSRVDescriptorAllocator();
	const auto& l_srvDescriptorHeap      = l_resourceContext.GetDescriptorHeapContext     ().GetSRVDescriptorHeap();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずテクスチャリソースの作成とアップロード処理を行えませんでした。");
		return false;
	}

	const auto& l_desc = CD3DX12_RESOURCE_DESC::Tex2D(a_metadata.format,
													  a_metadata.width,
													  static_cast<UINT>  (a_metadata.height),
													  static_cast<UINT16>(a_metadata.arraySize),
													  static_cast<UINT16>(a_metadata.mipLevels));

	ComPtr<ID3D12Heap> l_texHeap;
}

bool FWK::Graphics::TextureContext::AllocasteDefaultHeapTexture(const D3D12_RESOURCE_DESC& a_desc, ComPtr<ID3D12Heap>& a_outHeap, ComPtr<ID3D12Resource2> a_outResource)
{
	return false;
}

bool FWK::Graphics::TextureContext::CreateUploadBuffer(const UINT64 a_size, ComPtr<ID3D12Resource2>& a_outUpload)
{
	return false;
}

void FWK::Graphics::TextureContext::CreateSRV(const CommonStruct::TextureRecord& a_record)
{
}