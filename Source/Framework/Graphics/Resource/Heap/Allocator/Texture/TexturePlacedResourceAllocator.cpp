#include "TexturePlacedResourceAllocator.h"

FWK::Graphics::TexturePlacedResourceAllocator::TexturePlacedResourceAllocator() : 
	PlacedResourceAllocatorBase(k_textureHeapType, k_textureHeapFlags)
{}
FWK::Graphics::TexturePlacedResourceAllocator::~TexturePlacedResourceAllocator() = default;