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
		std::vector<ModelVertex>   m_vertexList = {};
		std::vector<std::uint32_t> m_indexList  = {};
	};

	struct ModelData final
	{
		std::vector<ModelMesh> m_meshList = {};
	};
}