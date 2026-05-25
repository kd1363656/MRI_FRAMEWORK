#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture;
}

namespace FWK::Converter
{
	class DepthStencilTextureJsonConverter final
	{
	public:

		 DepthStencilTextureJsonConverter() = default;
		~DepthStencilTextureJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::DepthStencilTexture& a_depthStencilTexture) const;

		nlohmann::json Serialize(const Graphics::DepthStencilTexture& a_depthStencilTexture) const;

	private:

		static constexpr std::string_view k_formatJsonKey = "Format";
		static constexpr std::string_view k_widthJsonKey  = "Width";
		static constexpr std::string_view k_heightJsonKey = "Height";
	};
}