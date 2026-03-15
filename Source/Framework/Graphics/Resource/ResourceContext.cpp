#include "ResourceContext.h"

void FWK::Graphics::ResourceContext::Init()
{
	m_descriptorHeapContext.Init();
	m_textureContext.Init();

	m_textureHeapAllocator.Init();
	m_textureUploader.Init();
}
bool FWK::Graphics::ResourceContext::Create(const Device& a_device)
{
	if (!m_descriptorHeapContext.Create(a_device))
	{
		assert(false && "ディスクリプタヒープコンテキストの作成処理に失敗しました。");
		return false;
	}

	if (!m_textureHeapAllocator.Create(k_defaultTextureHeapPageSize))
	{
		assert(false && "テクスチャヒープアロケータの作成処理に失敗しました。");
		return false;
	}

	if (!m_textureUploader.Create(a_device))
	{
		assert(false && "テクスチャアップローダーの作成処理に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::ResourceContext::Deserialize(const nlohmann::json& a_json)
{
	if (a_json.is_null()) { return; }


	if (a_json.contains("DescriptorHeapContext"))
	{
		m_descriptorHeapContext.Deserialize(a_json["DescriptorHeapContext"]);
	}
}
nlohmann::json FWK::Graphics::ResourceContext::Serialize()
{
	auto l_json = nlohmann::json();

	l_json["DescriptorHeapContext"] = m_descriptorHeapContext.Serialize();

	return l_json;
}

TextureID FWK::Graphics::ResourceContext::RegisterTexture(const Device& a_device, const std::string& a_sourcePath)
{
	if (a_sourcePath.empty())
	{
		assert(false && "登録対象のテクスチャパスが空文字のため、テクスチャ登録に失敗しました。");
		return CommonConstant::k_invalidTextureID;
	}

	const TextureID l_registeredTextureID = FindTextureID(a_sourcePath);
	
	if (l_registeredTextureID != CommonConstant::k_invalidTextureID)
	{
		return l_registeredTextureID;
	}

	auto&       l_srvDescriptorAllocator = m_descriptorHeapContext.GetMutableSRVDescriptorAllocator();
	const auto& l_srvDescriptorHeap      = m_descriptorHeapContext.GetSRVDescriptorHeap();

	const TextureID l_textureID = m_textureContext.RegisterTexture(l_srvDescriptorAllocator, a_sourcePath);

	if (l_textureID == CommonConstant::k_invalidTextureID)
	{
		assert(false && "TextureContextへの登録に失敗したため、テクスチャ登録に失敗しました。");
		return CommonConstant::k_invalidTextureID;
	}

	auto* l_textureRecord = m_textureContext.FindMutableTextureRecord(l_textureID);

	if (!l_textureRecord)
	{
		m_textureContext.UnregisterTexture(l_srvDescriptorAllocator, l_textureID);

		assert(false && "登録直後のTextureRecord取得に失敗したため、テクスチャ登録に失敗しました。");
		return CommonConstant::k_invalidTextureID;
	}

	if (!m_textureUploader.UploadTexture(a_device,
										 m_textureHeapAllocator,
										 l_srvDescriptorHeap,
										 a_sourcePath,
										 *l_textureRecord))
	{
		m_textureContext.UnregisterTexture(l_srvDescriptorAllocator, l_textureID);

		assert(false && "テクスチャアップロードに失敗したため、テクスチャ登録に失敗しました。");
		return CommonConstant::k_invalidTextureID;
	}

	return l_textureID;
}
bool FWK::Graphics::ResourceContext::UnregisterTexture(const TextureID a_textureID)
{
	auto& l_srvDescriptorAllocator = m_descriptorHeapContext.GetMutableSRVDescriptorAllocator();

	return m_textureContext.UnregisterTexture(l_srvDescriptorAllocator, a_textureID);
}

bool FWK::Graphics::ResourceContext::IsRegisteredTexture(const TextureID a_textureID) const
{
	return m_textureContext.IsRegisteredTexture(a_textureID);
}

TextureID FWK::Graphics::ResourceContext::FindTextureID(const std::string_view& a_sourcePath) const
{
	return m_textureContext.FindTextureID(a_sourcePath);
}
UINT FWK::Graphics::ResourceContext::FindTextureSRVIndex(const TextureID a_textureID) const
{
	return m_textureContext.FindSRVIndex(a_textureID);
}
const FWK::CommonStruct::TextureRecord* FWK::Graphics::ResourceContext::FindTextureRecord(const TextureID a_textureID) const
{
	return m_textureContext.FindTextureRecord(a_textureID);
}
FWK::CommonStruct::TextureRecord* FWK::Graphics::ResourceContext::FindMutableTextureRecord(const TextureID a_textureID)
{
	return m_textureContext.FindMutableTextureRecord(a_textureID);
}