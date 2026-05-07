#pragma once

namespace FWK::Struct
{
	struct StaticModelVertex final
	{
		TypeAlias::Math::Vector3 m_position = TypeAlias::Math::Vector3::Zero;
		TypeAlias::Math::Vector3 m_normal   = TypeAlias::Math::Vector3::Zero;
		TypeAlias::Math::Vector2 m_uv       = TypeAlias::Math::Vector2::Zero;
	};

	struct StaticModelMesh final
	{
		std::vector<StaticModelVertex> m_staticModelVertexList = {};
		std::vector<std::uint32_t>	   m_indexList			   = {};
	};

	struct StaticModelData final
	{
		std::vector<StaticModelMesh> m_staticModelMeshList = {};
	};

	struct ModelMeshlet final
	{
		std::uint32_t m_vertexOffset = 0U;
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