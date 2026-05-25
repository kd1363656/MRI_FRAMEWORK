#pragma once

namespace FWK::Graphics
{
	class SceneTexture;
}

namespace FWK::Converter
{
	class SceneTextureJsonConverter final
	{
	public:

		 SceneTextureJsonConverter() = default;
		~SceneTextureJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const;

		nlohmann::json Serialize(const Graphics::SceneTexture& a_sceneTexture) const;

	private:

		void DeserializeRenderTargetTextureRecord(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const;
		void DeserializeDepthStencilTextureRecord(const nlohmann::json& a_rootJson, Graphics::SceneTexture& a_sceneTexture) const;

		nlohmann::json SerializeRenderTargetTextureRecord(const Graphics::SceneTexture& a_sceneTexture) const;
		nlohmann::json SerializeDepthStencilTextureRecord(const Graphics::SceneTexture& a_sceneTexture) const;

		static constexpr std::string_view k_renderTargetTextureRecordListJsonKey = "RenderTargetListRecord";
		static constexpr std::string_view k_depthStencilTextureRecordListJsonKey = "DepthStencilTextureListRecord";

		static constexpr std::string_view k_renderTargetTextureTagJsonKey = "RenderTargetTag";
		static constexpr std::string_view k_depthStencilTextureTagJsonKey = "DepthStencilTextureTag";

		static constexpr std::string_view k_renderTargetTextureJsonKey = "RenderTarget";
		static constexpr std::string_view k_depthStencilTextureJsonKey = "DepthStencilTexture";
	};
}