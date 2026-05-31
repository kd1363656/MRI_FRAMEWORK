#include "ConstantBufferUploaderBase.h"

FWK::Graphics::ConstantBufferUploaderBase::ConstantBufferUploaderBase() : 
	m_uploadBuffer						 ({}),
	m_constantBufferUploaderJsonConverter({}),

	m_constantBufferTypeSize (k_invalidBufferTypeSize),
	m_createConstantBufferNUM(k_invalidCreateConstantBufferNUM)
{}
FWK::Graphics::ConstantBufferUploaderBase::ConstantBufferUploaderBase(const UINT64& a_constantBufferTypeSize) : 
	m_uploadBuffer						 ({}),
	m_constantBufferUploaderJsonConverter({}),

	m_constantBufferTypeSize (a_constantBufferTypeSize),
	m_createConstantBufferNUM(k_invalidCreateConstantBufferNUM)
{}
FWK::Graphics::ConstantBufferUploaderBase::~ConstantBufferUploaderBase() = default;

void FWK::Graphics::ConstantBufferUploaderBase::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_constantBufferUploaderJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::ConstantBufferUploaderBase::Create(const Device& a_device)
{
	FWK_ASSERT_RETURN_VALUE_IF(m_createConstantBufferNUM == k_invalidCreateConstantBufferNUM, "定数バッファの作成個数が0のため作成処理に失敗しました。", false)

	// 送る定数バッファの型サイズを256バイトにアライメントする
	const auto& l_alignedTypeSize    = Utility::Math::AlignUp(m_constantBufferTypeSize, k_constantBufferAlignment);
	const auto& l_constantBufferSize = m_createConstantBufferNUM * l_alignedTypeSize;
	
	FWK_ASSERT_RETURN_VALUE_IF(!m_uploadBuffer.Create(a_device, l_constantBufferSize), "定数バッファの生成処理に失敗しました。", false)

	return true;
}

void FWK::Graphics::ConstantBufferUploaderBase::BeginFrame()
{
	m_uploadBuffer.BeginFrame();
}

nlohmann::json FWK::Graphics::ConstantBufferUploaderBase::Serialize() const
{
	return m_constantBufferUploaderJsonConverter.Serialize(*this);
}

std::size_t FWK::Graphics::ConstantBufferUploaderBase::AllocateCurrentBufferIndex()
{
	const auto l_allocatedBufferIndex = m_uploadBuffer.AllocateCurrentBufferIndex();

	FWK_ASSERT_RETURN_VALUE_IF(l_allocatedBufferIndex >= m_createConstantBufferNUM, "取得した定数バッファ用インデックスが定数バッファの作成した個数を超えています", k_invalidConstantBufferIndex)

	return l_allocatedBufferIndex;
}