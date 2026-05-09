#pragma once

namespace FWK::Struct
{
	struct ModelVertex final
	{
		TypeAlias::Math::Vector3 m_position = {};
		TypeAlias::Math::Vector3 m_position = {};
		TypeAlias::Math::Vector2 m_uv		= {};
	};

	struct ModelMesh final
	{
		std::vector<ModelVertex>   m_modelVertexList = {};
		std::vector<std::uint32_t> m_indexList	     = {};
	};

}