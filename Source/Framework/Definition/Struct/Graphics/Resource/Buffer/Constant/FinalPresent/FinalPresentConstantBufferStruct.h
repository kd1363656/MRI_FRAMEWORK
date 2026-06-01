#pragma once

namespace FWK::Struct
{
	struct CBFinalPresentPass final
	{
		TypeAlias::StorageID     m_sceneColorTextureSRVIndex = 0U;
		TypeAlias::Math::Vector3 m_padding					 = {};
	};
}