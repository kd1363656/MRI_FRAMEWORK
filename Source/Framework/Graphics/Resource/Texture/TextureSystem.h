#pragma once

namespace FWK::Graphics
{
	class TextureSystem final
	{
	public:

		TextureSystem () = default;
		~TextureSystem() = default;

	private:

		TextureLoader m_textureLoader = {};
	};
}