#include "SpritePassConstantBuffer.h"

FWK::Graphics::SpritePassConstantBuffer::SpritePassConstantBuffer() : 
	ConstantBufferBase(sizeof(Struct::CBSpritePass))
{}
FWK::Graphics::SpritePassConstantBuffer::~SpritePassConstantBuffer() = default;