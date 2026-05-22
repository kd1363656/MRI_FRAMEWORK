#pragma once

namespace FWK::Struct
{
	// Lit用ライト定数
	struct CBLight final
	{
		Struct::DirectionalLight m_directionalLight = {};
		Struct::AmbientLight	 m_ambientLight     = {};
	};
}