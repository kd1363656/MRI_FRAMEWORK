#pragma once

namespace FWK::Graphics
{
	class UploadSystem;
}

namespace FWK::JsonConverter
{
	class UploadSystemJsonConverter
	{
	public:

		 UploadSystemJsonConverter() = default;
		~UploadSystemJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::UploadSystem& a_uploadSystem) const;

		nlohmann::json Serialize(const Graphics::UploadSystem& a_uploadSystem) const;

	private:

		static constexpr std::size_t k_defaultCopyCommandAllocatorListSize = 3ULL;
		static constexpr std::size_t k_minCopyCommandAllocatorListSize     = 1ULL;
	};
}