#pragma once

namespace FWK::Struct
{
	// ※ 注意
	// HLSLのcbufferは16バイト単位でパッキングされるため、
	// C++側の構造体レイアウトも16バイト境界を意識して定義すること
	struct CBSprite
	{
		TypeAlias::Math::Color m_color = Constant::k_noColor;

		TypeAlias::Math::Vector2 m_position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 m_scale    = TypeAlias::Math::Vector2::One;

		TypeAlias::Math::Vector2 m_pivot   = Constant::k_defaultPivot;
		TypeAlias::Math::Vector2 m_padding = TypeAlias::Math::Vector2::Zero;

		Struct::SpriteRECT m_sourceRECT = {};
	};
}