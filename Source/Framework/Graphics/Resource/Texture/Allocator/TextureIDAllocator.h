#pragma once

namespace FWK::Graphics
{
	class TextureIDAllocator final
	{
	public:

		 TextureIDAllocator() = default;
		~TextureIDAllocator() = default;

	private:



		TypeAlias::TextureID m_nextTextureID = k_initialNextTextureID;
	};
}