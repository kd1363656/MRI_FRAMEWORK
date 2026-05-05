#include "SpriteDrawConstantBuffer.h"

FWK::Graphics::SpriteDrawConstantBuffer::SpriteDrawConstantBuffer() : 
	ConstantBufferBase(sizeof(Struct::CBSprite))
{}
FWK::Graphics::SpriteDrawConstantBuffer::~SpriteDrawConstantBuffer() = default;