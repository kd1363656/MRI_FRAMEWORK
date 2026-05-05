#pragma once

namespace FWK::Graphics
{
	// ※注意
	// TextureIDを要素数としても使っています
	class TextureIDAllocator final
	{
	public:

		 TextureIDAllocator() = default;
		~TextureIDAllocator() = default;

		bool Create(const TypeAlias::TextureID a_textureIDCapacity);

		void Release(const TypeAlias::TextureID a_textureID);

		TypeAlias::TextureID Allocate();

	private:

		bool IsValidTextureID(const TypeAlias::TextureID a_textureID) const;

		static constexpr TypeAlias::TextureID k_firstNextTextureID = 0U;

		static constexpr bool k_unallocatedTextureIDState = false;
		static constexpr bool k_allocatedTextureIDState   = true;

		std::vector<bool> m_isAllocatedList = {};

		std::queue<TypeAlias::TextureID> m_freeTextureIDQueue = {};

		TypeAlias::TextureID m_textureIDCapacity = Constant::k_invalidTextureID;
		TypeAlias::TextureID m_nextTextureID     = k_firstNextTextureID;
	};
}