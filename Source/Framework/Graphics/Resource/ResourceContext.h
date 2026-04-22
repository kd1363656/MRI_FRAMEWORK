#pragma once

namespace FWK::Graphics
{
	class ResourceContext final
	{
	public:

		 ResourceContext() = default;
		~ResourceContext() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create	    (const Device&		   a_device);

		nlohmann::json Serialize() const;

		const auto& GetREFRTVDescriptorPool() const { return m_rtvDescriptorPool; }

		auto& GetREFMutableRTVDescriptorPool() { return m_rtvDescriptorPool; }

	private:

		DescriptorPool<RTVDescriptorHeap> m_rtvDescriptorPool = {};

		JsonConverter::ResourceContextJsonConverter m_resourceContextJsonConverter = {};
	};
}