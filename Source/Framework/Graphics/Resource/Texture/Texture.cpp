#include "Texture.h"

FWK::Graphics::Texture::Texture() : 
	m_textureID(Constant::k_invalidTextureID)
{}
FWK::Graphics::Texture::Texture(const Texture & a_other) : 
	m_textureID(a_other.m_textureID)
{
	AddTextureReference();
}
FWK::Graphics::Texture::Texture(Texture&& a_other) noexcept : 
	m_textureID(a_other.m_textureID)
{
	a_other.m_textureID = Constant::k_invalidTextureID;
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

	// コピー元のテクスチャIDを格納
	m_textureID = a_other.m_textureID;

	// 参照数の加算
	AddTextureReference();

	return *this;
}
FWK::Graphics::Texture& FWK::Graphics::Texture::operator=(Texture&& a_other) noexcept
{
	if (this == &a_other) { return *this; }

	// 所持しているテクスチャを破棄
	ReleaseTextureReference();

	// コピー元のテクスチャIDを格納
	m_textureID = a_other.m_textureID;

	// 参照元のテクスチャIDを無効化
	a_other.m_textureID = Constant::k_invalidTextureID;

	return *this;
}

void FWK::Graphics::Texture::Load(const std::filesystem::path& a_filePath)
{
	// 既に別のTextureIDを持っている場合は先に参照を外す
	ReleaseTextureReference();

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	const auto& l_device = l_graphicsManager.GetREFDevice();

		  auto& l_resourceContext    = l_graphicsManager.GetMutableREFResourceContext  ();
	const auto& l_gpuMemoryAllocator = l_resourceContext.GetREFGPUMemoryAllocator      ();
	      auto& l_textureSystem      = l_resourceContext.GetMutableREFTextureSystem    ();
	      auto& l_srvDescriptorPool  = l_resourceContext.GetMutableREFSRVDescriptorPool();
	
	m_textureID = l_textureSystem.LoadTextureForBatchUpload(l_device,
															l_gpuMemoryAllocator,
															a_filePath,
															l_srvDescriptorPool);

	if (m_textureID == Constant::k_invalidTextureID)
	{
		assert(false && "テクスチャ読み込みに失敗しました。");
		return;
	}
}

void FWK::Graphics::Texture::AddTextureReference() const
{
	if (m_textureID == Constant::k_invalidTextureID) { return; }

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetMutableREFTextureSystem  ();

	if (!l_textureSystem.AddTextureReference(m_textureID))
	{
		assert(false && "テクスチャ参照数加算に失敗しました。");
		return;
	}
}
void FWK::Graphics::Texture::ReleaseTextureReference()
{
	if (m_textureID == Constant::k_invalidTextureID) { return; }

	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	const auto& l_renderer		     = l_graphicsManager.GetREFRenderer   ();
	const auto& l_directCommandQueue = l_renderer.GetREFDirectCommandQueue();

	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();
	auto& l_textureSystem   = l_resourceContext.GetMutableREFTextureSystem  ();

	// 参照カウントを減らす
	if (!l_textureSystem.ReleaseTextureReference(l_directCommandQueue, m_textureID))
	{
		assert(false && "テクスチャ参照数解放に失敗しました。");
		return;
	}

	m_textureID = Constant::k_invalidTextureID;
}