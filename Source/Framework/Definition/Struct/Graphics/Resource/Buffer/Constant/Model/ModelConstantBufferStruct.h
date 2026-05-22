#pragma once

namespace FWK::Struct
{
	struct CBModelObject final
	{
		TypeAlias::Math::Matrix m_worldMatrix = TypeAlias::Math::Matrix::Identity;

		TypeAlias::StorageID m_baseColorTextureSRVIndex = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_normalTextureSRVIndex	= Constant::k_invalidStorageID;
		TypeAlias::StorageID m_vertexBufferSRVIndex     = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_meshletBufferSRVIndex    = Constant::k_invalidStorageID;

		TypeAlias::StorageID	 m_uniqueVertexIndexBufferSRVIndex = Constant::k_invalidStorageID;
		TypeAlias::StorageID	 m_primitiveIndexBufferSRVIndex    = Constant::k_invalidStorageID;
		TypeAlias::Math::Vector2 m_padding						   = {};
	};
}