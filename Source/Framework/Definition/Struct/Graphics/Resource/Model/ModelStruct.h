#pragma once

namespace FWK::Struct
{
	struct ModelVertex final
	{
		TypeAlias::Math::Vector3 m_position = {};
		TypeAlias::Math::Vector3 m_normal   = {};
		TypeAlias::Math::Vector2 m_uv		= {};
	};

	struct ModelMesh final
	{
		std::vector<ModelVertex>   m_modelVertexList = {};
		std::vector<std::uint32_t> m_indexList	     = {};
	};

	struct ModelMaterial final
	{
		std::wstring m_baseColorTextureFilePath = {};
		std::wstring m_normalTextureFilePath	= {};

		TypeAlias::StorageID m_baseColorTextureStorageID = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_normalTextureStorageID    = Constant::k_invalidStorageID;
	};

	struct ModelData final
	{
		std::vector<ModelMesh> m_modelMeshList = {};
	};
}