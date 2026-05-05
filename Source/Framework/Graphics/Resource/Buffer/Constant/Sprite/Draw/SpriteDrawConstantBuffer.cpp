#include "SpriteDrawConstantBuffer.h"

FWK::Graphics::SpriteDrawConstantBuffer::SpriteDrawConstantBuffer() : 
	ConstantBufferBase(sizeof(Struct::CBSpriteDraw))
{}
FWK::Graphics::SpriteDrawConstantBuffer::~SpriteDrawConstantBuffer() = default;