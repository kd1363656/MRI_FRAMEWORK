#pragma once

namespace FWK::Struct
{
	struct StaticModelRecord final : public AssetRecordBase
	{
		ModelData m_modelData = {};
	};
}