#include "Texture.h"

FWK::Graphics::Texture::Texture(const TextureID a_id) : 
	m_id(a_id)
{}
FWK::Graphics::Texture::Texture() :
	m_id(CommonConstant::k_invalidTextureID)
{}
FWK::Graphics::Texture::~Texture() = default;

bool FWK::Graphics::Texture::IsInvalid() const
{
	return m_id == CommonConstant::k_invalidTextureID;
}