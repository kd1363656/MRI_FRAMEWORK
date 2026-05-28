#include "Texture.h"

FWK::Graphics::Texture::Texture() : 
	m_storageID    (Constant::k_invalidStorageID),
	m_textureRecord({})
{}
FWK::Graphics::Texture::Texture(const Texture & a_other) : 
	m_storageID    (a_other.m_storageID),
	m_textureRecord(a_other.m_textureRecord)
{
	AddTextureReference();
}
FWK::Graphics::Texture::Texture(Texture&& a_other) noexcept : 
	m_storageID    (a_other.m_storageID),
	m_textureRecord(std::move(a_other.m_textureRecord))
{
	a_other.m_storageID = Constant::k_invalidStorageID;
	a_other.m_textureRecord.reset();
}
FWK::Graphics::Texture::~Texture()
{
	ReleaseTextureReference();
}

FWK::Graphics::Texture& FWK::Graphics::Texture::operator=(const Texture& a_other)
{
	if (this == &a_other) { return *this; }

	// 所持しているテクスチャを破棄
	ReleaseTextureReference();

	// コピー元と同じTextureRecordを参照する
	m_storageID     = a_other.m_storageID;
	m_textureRecord = a_other.m_textureRecord;

	// 参照数の加算
	AddTextureReference();

	return *this;
}
FWK::Graphics::Texture& FWK::Graphics::Texture::operator=(Texture&& a_other) noexcept
{
	if (this == &a_other) { return *this; }

	// 所持しているテクスチャを破棄
	ReleaseTextureReference();

	// ムーブでは参照数を増やさず、参照先だけ移す
	m_storageID     = a_other.m_storageID;
	m_textureRecord = std::move(a_other.m_textureRecord);

	// 参照元のストレージIDを無効化
	a_other.m_storageID = Constant::k_invalidStorageID;
	a_other.m_textureRecord.reset();

	return *this;
}

void FWK::Graphics::Texture::Load(const std::filesystem::path& a_filePath)
{
	// 既に別のStorageIDを持っている場合は先に参照を外す
	ReleaseTextureReference();

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	const auto& l_device = l_graphicsManager.GetREFDevice();

		  auto& l_resourceContext    = l_graphicsManager.GetMutableREFResourceContext  ();
	const auto& l_gpuMemoryAllocator = l_resourceContext.GetREFGPUMemoryAllocator      ();
	      auto& l_textureSystem      = l_resourceContext.GetMutableREFTextureSystem    ();
	      auto& l_srvDescriptorPool  = l_resourceContext.GetMutableREFSRVDescriptorPool();
	
	const auto& l_textureLoadResult = l_textureSystem.LoadTextureForBatchUpload(l_device,
																				l_gpuMemoryAllocator,
																				a_filePath,
																				l_srvDescriptorPool);

	if (l_textureLoadResult.m_storageID == Constant::k_invalidStorageID) 
	{
		SetupDefaultTexture(Enum::DefaultTextureType::BaseColor);
		return;
	}

	if (l_textureLoadResult.m_textureRecord.expired())
	{
		SetupDefaultTexture(Enum::DefaultTextureType::BaseColor);
		return;
	}

	m_storageID     = l_textureLoadResult.m_storageID;
	m_textureRecord = l_textureLoadResult.m_textureRecord;
}

void FWK::Graphics::Texture::SetupDefaultTexture(const Enum::DefaultTextureType a_defaultTextureType)
{
	// 通常テクスチャを使っている場合は先に参照を外す
	ReleaseTextureReference();

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetMutableREFTextureSystem  ();
	
	const auto& l_defaultTextureRecord = l_textureSystem.FindVALDefaultTextureRecord(a_defaultTextureType);

	if (l_defaultTextureRecord.expired())
	{
		assert(false && "DefaultTextureRecordが無効のため、デフォルトテクスチャ設定に失敗しました。");
		return;
	}

	// デフォルトテクスチャはAssetStorage管理ではないため、StorageIDは無効値のままにする
	m_storageID     = Constant::k_invalidStorageID;
	m_textureRecord = l_defaultTextureRecord;
}

void FWK::Graphics::Texture::AddTextureReference() const
{
	if (m_storageID == Constant::k_invalidStorageID) { return; }

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetMutableREFTextureSystem  ();

	if (!l_textureSystem.AddTextureReference(m_textureRecord))
	{
		assert(false && "テクスチャ参照数加算に失敗しました。");
		return;
	}
}
void FWK::Graphics::Texture::ReleaseTextureReference()
{
	if (m_storageID == Constant::k_invalidStorageID) 
	{
		m_textureRecord.reset();
		return; 
	}

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	const auto& l_renderer		     = l_graphicsManager.GetREFRenderer   ();
	const auto& l_directCommandQueue = l_renderer.GetREFDirectCommandQueue();

	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetMutableREFTextureSystem  ();
	
	// 参照カウントを減らす
	if (auto& l_deferredResourceReleaseQueue = l_resourceContext.GetMutableREFDeferredResourceReleaseQueue();
		!l_textureSystem.ReleaseTextureReference(m_textureRecord, l_directCommandQueue, l_deferredResourceReleaseQueue))
	{
		assert(false && "テクスチャ参照数解放に失敗しました。");
		return;
	}

	m_storageID = Constant::k_invalidStorageID;
	m_textureRecord.reset();
}