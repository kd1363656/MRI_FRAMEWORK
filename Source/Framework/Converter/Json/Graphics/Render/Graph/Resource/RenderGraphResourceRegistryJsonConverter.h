#pragma once

namespace FWK::Graphics
{
	class RenderGraphResourceRegistry;
}

namespace FWK::Converter
{
	class RenderGraphResourceRegistryJsonConverter
	{
	public:
		
		 RenderGraphResourceRegistryJsonConverter() = default;
		~RenderGraphResourceRegistryJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const;

		nlohmann::json Serialize(const Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const;

	private:

		void DeserializeRenderTargetTextureRecord(const nlohmann::json& a_rootJson, Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const;
		void DeserializeDepthStencilTextureRecord(const nlohmann::json& a_rootJson, Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const;

		nlohmann::json SerializeRenderTargetTextureRecord(const Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const;
		nlohmann::json SerializeDepthStencilTextureRecord(const Graphics::RenderGraphResourceRegistry& a_renderGraphResourceRegistry) const;

		static constexpr std::string_view k_renderTargetTextureRecordListJsonKey = "RenderTargetListRecord";
		static constexpr std::string_view k_depthStencilTextureRecordListJsonKey = "DepthStencilTextureListRecord";

		static constexpr std::string_view k_renderTargetTextureTagJsonKey = "RenderTargetTextureTag";
		static constexpr std::string_view k_depthStencilTextureTagJsonKey = "DepthStencilTextureTag";

		static constexpr std::string_view k_renderTargetTextureJsonKey = "RenderTargetTexture";
		static constexpr std::string_view k_depthStencilTextureJsonKey = "DepthStencilTexture";
	};
}