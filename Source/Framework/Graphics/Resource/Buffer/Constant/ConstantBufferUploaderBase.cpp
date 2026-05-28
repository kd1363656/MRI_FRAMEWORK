#include "ConstantBufferUploaderBase.h"

FWK::Graphics::ConstantBufferUploaderBase::ConstantBufferUploaderBase() : 
	m_uploadConstantBuffer				 ({}),
	m_constantBufferUploaderJsonConverter({}),

	m_constantBufferTypeSize (k_invalidBufferTypeSize),
	m_createConstantBufferNUM(Constant::k_invalidCreateConstantBufferNUM)
{}
FWK::Graphics::ConstantBufferUploaderBase::ConstantBufferUploaderBase(const UINT64& a_constantBufferTypeSize) : 
	m_uploadConstantBuffer				 ({}),
	m_constantBufferUploaderJsonConverter({}),

	m_constantBufferTypeSize (a_constantBufferTypeSize),
	m_createConstantBufferNUM(Constant::k_invalidCreateConstantBufferNUM)
{}
FWK::Graphics::ConstantBufferUploaderBase::~ConstantBufferUploaderBase() = default;

void FWK::Graphics::ConstantBufferUploaderBase::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_constantBufferUploaderJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::ConstantBufferUploaderBase::Create(const Device& a_device)
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

void FWK::Graphics::ConstantBufferUploaderBase::BeginFrame()
{
	m_uploadConstantBuffer.BeginFrame();
}

nlohmann::json FWK::Graphics::ConstantBufferUploaderBase::Serialize() const
{
	return m_constantBufferUploaderJsonConverter.Serialize(*this);
}

std::size_t FWK::Graphics::ConstantBufferUploaderBase::AllocateCurrentBufferIndex()
{
	const auto l_allocatedBufferIndex = m_uploadConstantBuffer.AllocateCurrentBufferIndex();

	if (l_allocatedBufferIndex >= m_createConstantBufferNUM)
	{
		assert(false && "取得した定数バッファ用インデックスが定数バッファの作成した個数を超えています");
		return Constant::k_invalidConstantBufferIndex;
	}

	return l_allocatedBufferIndex;
}