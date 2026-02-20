#pragma once

namespace FWK
{
	class Window;
}

namespace FWK::Converter
{
	class WindowJsonConverter final : public JsonConverterBase 
	{
	public:

		explicit WindowJsonConverter(Window& a_window);
		~WindowJsonConverter        ()                 override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()                                 override;

	private:

		Window& m_window;
	};
}