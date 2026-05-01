#pragma once

namespace FWK
{
	class Window;
}

namespace FWK::JsonConverter
{
	class WindowJsonConverter final
	{
	public:

		 WindowJsonConverter() = default;
		~WindowJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Window& a_window) const;

		nlohmann::json Serialize(const Window& a_window) const;

	private:

		static constexpr std::string_view k_windowStyleTagJsonKey = "WindowStyleTag";
		static constexpr std::string_view k_widthJsonKey          = "Width";
		static constexpr std::string_view k_heightJsonKey         = "Height";
	};
}