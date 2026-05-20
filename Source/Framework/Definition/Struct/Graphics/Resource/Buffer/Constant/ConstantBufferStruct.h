#pragma once

namespace FWK::Struct
{
	// ※ 注意
	// HLSLのConstantBufferは16バイト単位でパッキングされるため、
	// C++側の構造体レイアウトも16バイト境界を意識して定義すること
	struct CBSpriteDraw final
	{
		TypeAlias::Math::Color m_color = Constant::k_noColor;

		TypeAlias::Math::Vector2 m_position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 m_scale    = TypeAlias::Math::Vector2::One;

		TypeAlias::Math::Vector2 m_pivot   = Constant::k_defaultPivot;
		TypeAlias::Math::Vector2 m_padding = TypeAlias::Math::Vector2::Zero;

		Struct::SpriteRECT m_sourceRECT = {};
	};

	// 2D画像の共通パス
	struct CBSpritePass final
	{
		TypeAlias::Math::Matrix m_projectionMatrix = TypeAlias::Math::Matrix::Identity;
	};

	// カメラ
	struct CBCamera final
	{
		TypeAlias::Math::Matrix m_viewMatrix           = TypeAlias::Math::Matrix::Identity;
		TypeAlias::Math::Matrix m_projectionMatrix     = TypeAlias::Math::Matrix::Identity;
		TypeAlias::Math::Matrix m_viewProjectionMatrix = TypeAlias::Math::Matrix::Identity;
	};

	// モデル
	struct CBModelObject final
	{
		TypeAlias::Math::Matrix m_worldMatrix = TypeAlias::Math::Matrix::Identity;
	};

	// マテリアル
	struct CBModelMaterial final
	{
		TypeAlias::Math::Color m_baseColorFactor = Constant::k_defaultModelBaseColorFactor;

		TypeAlias::Math::Vector2 m_padding		   = {};
		float                    m_metallicFactor  = Constant::k_defaultModelMetallicFactor;
		float                    m_roughnessFactor = Constant::k_defaultModelRoughnessFactor;

		std::uint32_t m_useBaseColorTexture = Constant::k_disableModelTexture;
		std::uint32_t m_useNormalTexture    = Constant::k_disableModelTexture;
		std::uint32_t m_useRoughnessTexture = Constant::k_disableModelTexture;
		std::uint32_t m_useMetallicTexture  = Constant::k_disableModelTexture;
	};
}