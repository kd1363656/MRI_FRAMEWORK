#pragma once

namespace FWK::Struct
{
	// ※ 注意
	// HLSLのConstantBufferは16バイト単位でパッキングされるため、
	// C++側の構造体レイアウトも16バイト境界を意識して定義すること
	struct CBSpriteObject final
	{
		TypeAlias::Math::Color m_color = Constant::k_noColor;

		TypeAlias::Math::Vector2 m_position = TypeAlias::Math::Vector2::Zero;
		TypeAlias::Math::Vector2 m_scale    = TypeAlias::Math::Vector2::One;

		TypeAlias::Math::Vector2 m_pivot	    = Constant::k_defaultPivot;
		TypeAlias::Math::Vector2 m_firstPadding = TypeAlias::Math::Vector2::Zero;

		Struct::SpriteRECT m_sourceRECT = {};

		std::uint32_t			 m_baseColorTextureIndex = Constant::k_invalidStorageID;
		TypeAlias::Math::Vector3 m_secondPadding		 = {};
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

		std::uint32_t m_baseColorTextureIndex        = Constant::k_invalidStorageID;
		std::uint32_t m_normalTextureIndex			 = Constant::k_invalidStorageID;
		std::uint32_t m_vertexBufferIndex            = Constant::k_invalidStorageID;
		std::uint32_t m_meshletBufferIndex           = Constant::k_invalidStorageID;

		std::uint32_t			 m_uniqueVertexIndexBufferIndex = Constant::k_invalidStorageID;
		std::uint32_t			 m_primitiveIndexBufferIndex    = Constant::k_invalidStorageID;
		TypeAlias::Math::Vector2 m_padding					    = {};
	};
}