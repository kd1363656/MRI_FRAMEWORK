#pragma once

namespace FWK::Struct
{
	// 平行光源
	struct DirectionalLight final
	{
		TypeAlias::Math::Vector3 m_direction = TypeAlias::Math::Vector3(-0.3F, 0.0F, 0.3F);
		float					 m_intensity = 1.0F;

		TypeAlias::Math::Vector3 m_color   = TypeAlias::Math::Vector3(0.80F, 0.80F, 0.80F);
		float					 m_padding = 0.0F;
	};

	// 環境光
	struct AmbientLight final
	{
		TypeAlias::Math::Vector3 m_color	 = TypeAlias::Math::Vector3::One;
		float					 m_intensity = 0.25F;
	};
}