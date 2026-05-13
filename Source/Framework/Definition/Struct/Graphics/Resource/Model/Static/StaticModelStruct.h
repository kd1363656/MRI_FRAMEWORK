#pragma once

namespace FWK::Struct
{
	struct StaticModelRecord final : public AssetRecordBase
	{
		std::shared_ptr<ModelData> m_modelData = nullptr;
	};
}