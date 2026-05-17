#pragma once

namespace FWK::Struct
{
	struct SpriteStandardDrawCommand final
	{
		std::weak_ptr<Struct::TextureRecord> m_textureRecord = {};

		TypeAlias::Math::Color m_color = {};

		TypeAlias::Math::Vector2 m_position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 m_scale    = TypeAlias::Math::Vector2::One;
		TypeAlias::Math::Vector2 m_pivot    = Constant::k_defaultPivot;

		SpriteRECT m_sourceRECT = {};
	};

	struct StaticModelUnLitStandardDrawCommand final
	{
		std::weak_ptr<Struct::StaticModelRecord> m_staticModelRecord = {};

		TypeAlias::Math::Matrix m_worldMatrix = TypeAlias::Math::Matrix::Identity;
	};
}