#pragma once

namespace FWK::Struct
{
	struct CBSprite
	{
		TypeAlias::Math::Color color = Constant::k_noColor;

		TypeAlias::Math::Vector2 position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 scale    = TypeAlias::Math::Vector2::One;

		TypeAlias::Math::Vector2 pivot   = Constant::k_defaultPivot;
		TypeAlias::Math::Vector2 padding = TypeAlias::Math::Vector2::Zero;

		Struct::SpriteRECT sourceRECT = {};
	};
}