#pragma once

namespace FWK::Graphics
{
	class RenderGraphFrameResourceRegistry;
}

namespace FWK::Converter
{
	class RenderGraphFrameResourceRegistryJsonConverter
	{
	public:
		
		 RenderGraphFrameResourceRegistryJsonConverter() = default;
		~RenderGraphFrameResourceRegistryJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry) const;

		nlohmann::json Serialize(const Graphics::RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry) const;

	private:

		void DeserializeRenderTargetTextureRecord(const nlohmann::json& a_rootJson, Graphics::RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry) const;
		void DeserializeDepthStencilTextureRecord(const nlohmann::json& a_rootJson, Graphics::RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry) const;

		nlohmann::json SerializeRenderTargetTextureRecord(const Graphics::RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry) const;
		nlohmann::json SerializeDepthStencilTextureRecord(const Graphics::RenderGraphFrameResourceRegistry& a_renderGraphFrameResourceRegistry) const;

		static constexpr std::string_view k_renderTargetTextureRecordListJsonKey = "RenderTargetListRecord";
		static constexpr std::string_view k_depthStencilTextureRecordListJsonKey = "DepthStencilTextureListRecord";

		static constexpr std::string_view k_renderTargetTextureTagJsonKey = "RenderTargetTextureTag";
		static constexpr std::string_view k_depthStencilTextureTagJsonKey = "DepthStencilTextureTag";

		static constexpr std::string_view k_renderTargetTextureJsonKey = "RenderTargetTexture";
		static constexpr std::string_view k_depthStencilTextureJsonKey = "DepthStencilTexture";
	};
}