#pragma once

namespace FWK::Graphics
{
	class TexturePlacedResourceAllocator final : public PlacedResourceAllocatorBase
	{
	public:

		 TexturePlacedResourceAllocator();
		~TexturePlacedResourceAllocator() override;

	private:

		static constexpr D3D12_HEAP_TYPE  k_textureHeapType  = D3D12_HEAP_TYPE_DEFAULT;
		static constexpr D3D12_HEAP_FLAGS k_textureHeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	};
}