#pragma once

namespace FWK::Graphics
{
	class Renderer;
}

namespace FWK::Graphics
{
	class PipelineState final
	{
	public:

		 PipelineState() = default;
		~PipelineState() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     (const Device&         a_device, const ShaderCompiler& a_shaderCompiler, const Renderer& a_renderer);

		nlohmann::json Serialize() const;

		void EnsureShader(std::shared_ptr<Shader>& a_shader) const;

	private:

		std::shared_ptr<Shader> m_amplification = nullptr;
		std::shared_ptr<Shader> m_pixelShader   = nullptr;

		D3D12_RASTERIZER_DESC    m_rasterizerDesc   = {};
		D3D12_BLEND_DESC         m_blendDesc        = {};
		D3D12_DEPTH_STENCIL_DESC m_depthStencilDesc = {};
		DXGI_SAMPLE_DESC	     m_sampleDesc       = {};

		D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		DXGI_FORMAT m_dsvFormat = DXGI_FORMAT_UNKNOWN;

		UINT m_sampleMask = UINT_MAX;

		Shader m_meshShader = {};

		TypeAlias::ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;

		TypeAlias::TypeTag m_useRootSignatureTag = Constant::k_invalidTypeTag;

		std::vector<DXGI_FORMAT> m_rtvFormatList = {};
	};
}