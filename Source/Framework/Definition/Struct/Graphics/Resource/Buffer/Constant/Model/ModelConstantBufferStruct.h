#pragma once

namespace FWK::Struct
{
	struct CBModelObject final
	{
		TypeAlias::Math::Matrix m_worldMatrix = TypeAlias::Math::Matrix::Identity;

		std::uint32_t m_baseColorTextureIndex = Constant::k_invalidStorageID;
		std::uint32_t m_normalTextureIndex	  = Constant::k_invalidStorageID;
		std::uint32_t m_vertexBufferIndex     = Constant::k_invalidStorageID;
		std::uint32_t m_meshletBufferIndex    = Constant::k_invalidStorageID;

		std::uint32_t			 m_uniqueVertexIndexBufferIndex = Constant::k_invalidStorageID;
		std::uint32_t			 m_primitiveIndexBufferIndex    = Constant::k_invalidStorageID;
		TypeAlias::Math::Vector2 m_padding					    = {};
	};
}