#pragma once

namespace FWK::Graphics
{
	class Camera;
}

namespace FWK::Struct
{
	// スプライト用
	struct SpriteStandardDrawCommand final
	{
		std::weak_ptr<Struct::TextureRecord> m_textureRecord = {};

		TypeAlias::Math::Color m_color = {};

		TypeAlias::Math::Vector2 m_position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 m_scale    = TypeAlias::Math::Vector2::One;
		TypeAlias::Math::Vector2 m_pivot    = Constant::k_defaultPivot;

		SpriteRECT m_sourceRECT = {};
	};

	// 静的モデル標準描画用
	struct StaticModelStandardDrawCommand final
	{
		std::weak_ptr<Struct::StaticModelRecord> m_staticModelRecord = {};

		TypeAlias::Math::Matrix m_worldMatrix = TypeAlias::Math::Matrix::Identity;
	};

	struct StaticModelStandardPassConstant final
	{
		std::weak_ptr<Graphics::Camera> m_camera = {};
	};
}