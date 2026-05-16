#pragma once

namespace FWK::Graphics 
{
	class StaticModelRecordReleaser final
	{
	public:

		 StaticModelRecordReleaser() = default;
		~StaticModelRecordReleaser() = default;

		bool ReleaseRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_textureRecord) const;
	};
}