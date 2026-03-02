#pragma once

namespace FWK::Graphics
{
	class Hardware final
	{
	public:

		Hardware ();
		~Hardware();

		void Init  ();
		bool Create();
		
		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize  ();

		const auto& GetFactory() const { return m_factory; }
		const auto& GetDevice () const { return m_device; }

	private:

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		Factory m_factory;
		Device  m_device;
	};
}