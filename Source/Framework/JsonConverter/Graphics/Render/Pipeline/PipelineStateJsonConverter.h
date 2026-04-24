#pragma once

namespace FWK::Graphics
{
	class PipelineState;
}

namespace FWK::Converter
{
	class PipelineStateJsonConverter final
	{
	public:
		
		 PipelineStateJsonConverter() = default;
		~PipelineStateJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::PipelineState& a_pipelineState) const;

		nlohmann::json Serialize(const Graphics::PipelineState& a_pipelineState) const;

	private:

		void DeserializeOptionalShader(const nlohmann::json& a_rootJson, std::shared_ptr<Graphics::Shader>& a_shader) const;

		void DeserializeRasterizerDesc    (const nlohmann::json& a_rootJson, Graphics::PipelineState& a_pipelineState) const;
		void DeserializeBlendDesc         (const nlohmann::json& a_rootJson, Graphics::PipelineState& a_pipelineState) const;
		void DeserializeDepthStencilDesc  (const nlohmann::json& a_rootJson, Graphics::PipelineState& a_pipelineState) const;
		void DeserializeRTVFormatList     (const nlohmann::json& a_rootJson, Graphics::PipelineState& a_pipelineState) const;
		void DeserializeSampleDesc        (const nlohmann::json& a_rootJson, Graphics::PipelineState& a_pipelineState) const;

		nlohmann::json SerializeRasterizerDesc    (const Graphics::PipelineState&    a_pipelineState)      const;
		nlohmann::json SerializeBlendDesc         (const Graphics::PipelineState&    a_pipelineState)      const;
		nlohmann::json SerializeDepthStencilDesc  (const Graphics::PipelineState&    a_pipelineState)      const;
		nlohmann::json SerializeDepthStencilOPDesc(const D3D12_DEPTH_STENCILOP_DESC& a_depthStencilOPDesc) const;
		nlohmann::json SerializeRTVFormatList     (const Graphics::PipelineState&    a_pipelineState)      const;
		nlohmann::json SerializeSampleDesc        (const Graphics::PipelineState&    a_pipelineState)      const;

		void DeserializeDepthStencilOpDesc(const nlohmann::json& a_rootJson, D3D12_DEPTH_STENCILOP_DESC& a_depthStencilOPDesc) const;

		void EnsureShader(std::shared_ptr<Graphics::Shader>& a_shader) const;

		static constexpr UINT k_defaultSampleCount   = 1U;
		static constexpr UINT k_defaultSampleQuality = 0U;
		static constexpr UINT k_noForcedSampleCount  = 0U;
	};
}