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
}