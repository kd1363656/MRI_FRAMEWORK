#include "ConstantBufferBase.h"

FWK::Graphics::ConstantBufferBase::ConstantBufferBase() : 
	m_uploadConstantBuffer       ({}),
	m_constantBufferJsonConverter({}),

	m_constantBufferTypeSize (k_invalidBufferTypeSize),
	m_createConstantBufferNUM(Constant::k_invalidCreateConstantBufferNUM)
{}
FWK::Graphics::ConstantBufferBase::ConstantBufferBase(const UINT64& a_constantBufferTypeSize) : 
	m_uploadConstantBuffer       ({}),
	m_constantBufferJsonConverter({}),

	m_constantBufferTypeSize (a_constantBufferTypeSize),
	m_createConstantBufferNUM(Constant::k_invalidCreateConstantBufferNUM)
{}
FWK::Graphics::ConstantBufferBase::~ConstantBufferBase() = default;

void FWK::Graphics::ConstantBufferBase::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_constantBufferJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::ConstantBufferBase::Create(const Device& a_device)
{
	if (m_createConstantBufferNUM == Constant::k_invalidCreateConstantBufferNUM)
	{
		assert(false && "定数バッファの作成個数が0のため作成処理に失敗しました。");
		return false;
	}

	// 送る定数バッファの型サイズを256バイトにアライメントする
	const auto& l_alignedTypeSize = Utility::Math::AlignUp(m_constantBufferTypeSize, Constant::k_constantBufferAlignment);
	
	if (const auto& l_constantBufferSize = m_createConstantBufferNUM * l_alignedTypeSize;
		!m_uploadConstantBuffer.Create(a_device, l_constantBufferSize))
	{
		assert(false && "スプライト描画用定数バッファの生成処理に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::ConstantBufferBase::BeginFrame()
{
	m_uploadConstantBuffer.BeginFrame();
}

nlohmann::json FWK::Graphics::ConstantBufferBase::Serialize() const
{
	return m_constantBufferJsonConverter.Serialize(*this);
}

std::size_t FWK::Graphics::ConstantBufferBase::AllocateCurrentBufferIndex()
{
	return m_uploadConstantBuffer.AllocateCurrentBufferIndex();
}