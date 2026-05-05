#include "ConstantBufferBase.h"

FWK::Graphics::ConstantBufferBase::ConstantBufferBase() = default;
FWK::Graphics::ConstantBufferBase::ConstantBufferBase(const UINT64& a_constantBufferTypeSize) : 
	m_constantBufferTypeSize (a_constantBufferTypeSize),
	m_createConstantBufferNUM(Constant::k_invalidCreateConstantBufferNUM),

	m_uploadConstantBuffer({})
{}

FWK::Graphics::ConstantBufferBase::~ConstantBufferBase()
{
}

void FWK::Graphics::ConstantBufferBase::Deserialize(const nlohmann::json& a_rootJson)
{
}
bool FWK::Graphics::ConstantBufferBase::Create(const Device& a_device)
{
	return false;
}

nlohmann::json FWK::Graphics::ConstantBufferBase::Serialize() const
{
	return nlohmann::json();
}
