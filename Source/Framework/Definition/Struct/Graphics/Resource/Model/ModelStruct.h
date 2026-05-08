#pragma once

namespace FWK::Struct
{
	struct ModelVertex final
	{
		TypeAlias::Math::Vector3 m_position = TypeAlias::Math::Vector3::Zero;
		TypeAlias::Math::Vector3 m_normal   = TypeAlias::Math::Vector3::Zero;
		TypeAlias::Math::Vector2 m_uv       = TypeAlias::Math::Vector2::Zero;
	};

	struct ModelMesh final
	{
		std::vector<ModelVertex>   m_modelVertexList = {};
		std::vector<std::uint32_t> m_indexList		 = {};
	};

	struct ModelData final
	{
		std::vector<ModelMesh> m_modelMeshList = {};
	};

	struct ModelMeshlet final
	{
		std::uint32_t m_vertexOffset   = 0U;
		std::uint32_t m_triangleOffset = 0U;

		std::uint32_t m_vertexCount   = 0U;
		std::uint32_t m_triangleCount = 0U;
	};

	struct ModelMeshletData final
	{
		std::vector<ModelMeshlet>  m_meshletList		   = {};
		std::vector<std::uint32_t> m_uniqueVertexIndexList = {};
		std::vector<std::uint8_t>  m_primitiveIndexList	   = {};
	};
}