#pragma once

namespace FWK::Graphics
{
	class Renderer;
}

namespace FWK::JsonConverter
{
	class RendererJsonConverter final
	{
	public:

 		 RendererJsonConverter() = default;
		~RendererJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const;

		nlohmann::json Serialize(const Graphics::Renderer& a_renderer) const;

	private: 

		void DeserializeFrameResourceList(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const;
		void DeserializeRootSignatureMap (const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const;

		nlohmann::json SerializeFrameResourceList(const Graphics::Renderer& a_renderer) const;
		nlohmann::json SerializeRootSignatureMap (const Graphics::Renderer& a_renderer) const;
	};
}