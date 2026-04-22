#pragma once

namespace FWK::Graphics
{
	class Renderer;
}

namespace FWK::Converter
{
	class RendererJsonConverter final
	{
	public:

 		 RendererJsonConverter() = default;
		~RendererJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const;

		nlohmann::json Serialize(const Graphics::Renderer& a_renderer) const;
	};
}