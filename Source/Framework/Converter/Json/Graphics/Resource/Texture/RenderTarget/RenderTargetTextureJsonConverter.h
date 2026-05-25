#pragma once

namespace FWK::Graphcis
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
	};
}