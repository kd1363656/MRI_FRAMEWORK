#pragma once

namespace FWK::Graphics
{
	class RenderTargetTexture;
}

namespace FWK::Converter
{
	class RenderTargetTextureJsonConverter final
	{
	public:

		 RenderTargetTextureJsonConverter() = default;
		~RenderTargetTextureJsonConverter() = default; 

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderTargetTexture& a_renderTargetTexture) const;

		nlohmann::json Serialize(const Graphics::RenderTargetTexture& a_renderTargetTexture) const;

	private:

		static constexpr std::string_view k_clearColorJsonKey = "ClearColor";
		static constexpr std::string_view k_formatJsonKey	  = "Format";
		static constexpr std::string_view k_widthJsonKey	  = "Width";
		static constexpr std::string_view k_heightJsonKey	  = "Height";
	};
}