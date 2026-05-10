#pragma once

namespace FWK::Struct
{
	struct SpriteDrawCommand final
	{
		TypeAlias::Math::Color m_color = {};

		TypeAlias::Math::Vector2 m_position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 m_scale    = TypeAlias::Math::Vector2::One;
		TypeAlias::Math::Vector2 m_pivot    = Constant::k_defaultPivot;

		SpriteRECT m_sourceRECT = {};

		TypeAlias::StorageID m_storageID = Constant::k_invalidStorageID;
	};
}