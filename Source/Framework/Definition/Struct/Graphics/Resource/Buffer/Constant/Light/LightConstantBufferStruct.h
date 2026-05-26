#pragma once

namespace FWK::Struct
{
	// Lit用ライト定数
	struct CBLight final
	{
		Struct::DirectionalLight m_directionalLight = {};
		float					 m_padding		    = 0.0F;

		Struct::AmbientLight m_ambientLight = {};
	};
}