#pragma once

namespace FWK::Struct
{
	struct CBFinalPresentPass final
	{
		TypeAlias::StorageID     m_sceneColorTextureSRVIndex = Constant::k_invalidStorageID;
		TypeAlias::Math::Vector3 m_padding					 = {};
	};
}