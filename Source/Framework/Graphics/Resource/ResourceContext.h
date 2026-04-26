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
		const auto& GetREFSRVDescriptorPool() const { return m_srvDescriptorPool; }

		auto& GetMutableREFRTVDescriptorPool() { return m_rtvDescriptorPool; }
		auto& GetMutableREFSRVDescriptorPool() { return m_srvDescriptorPool; }

	private:

		DescriptorPool<RTVDescriptorHeap> m_rtvDescriptorPool = {};
		DescriptorPool<SRVDescriptorHeap> m_srvDescriptorPool = {};
		
		UploadSystem  m_uploadSystem  = {};
		TextureSystem m_textureSystem = {};

		JsonConverter::ResourceContextJsonConverter m_resourceContextJsonConverter = {};
	};
}