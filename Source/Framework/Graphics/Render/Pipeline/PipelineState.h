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

		void AddRTVFormat(const DXGI_FORMAT a_format);

		void SetRasterizerDesc  (const D3D12_RASTERIZER_DESC&    a_set) { m_rasterizerDesc   = a_set; }
		void SetBlendDesc       (const D3D12_BLEND_DESC&         a_set) { m_blendDesc        = a_set; }
		void SetDepthStencilDesc(const D3D12_DEPTH_STENCIL_DESC& a_set) { m_depthStencilDesc = a_set; }
		void SetSampleDesc      (const DXGI_SAMPLE_DESC&         a_set) { m_sampleDesc       = a_set; }

		void SetPrimitiveTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE a_set) { m_primitiveTopologyType = a_set; }

		void SetDSVFormat(const DXGI_FORMAT a_set) { m_dsvFormat = a_set; }

		void SetSampleMask(const UINT a_set) { m_sampleMask = a_set; }

		void SetUseRootSignatureTag(const TypeAlias::TypeTag a_set) { m_useRootSignatureTag = a_set; }

		const auto& GetREFPipelineState() const { return m_pipelineState; }

		const auto& GetREFRasterizerDesc  () const { return m_rasterizerDesc; }
		const auto& GetREFBlendDesc       () const { return m_blendDesc; }
		const auto& GetREFDepthStencilDesc() const { return m_depthStencilDesc; }
		const auto& GetREFSampleDesc      () const { return m_sampleDesc; }

		const auto& GetREFAmplificationShader() const { return m_amplificationShader; }
		const auto& GetREFMeshShader         () const { return m_meshShader; }
		const auto& GetREFPixelShader        () const { return m_pixelShader; }

		const auto& GetREFRTVFormatList() const { return m_rtvFormatList; }

		auto& GetMutableREFAmplificationShader() { return m_amplificationShader; }
		auto& GetMutableREFMeshShader         () { return m_meshShader; }
		auto& GetMutableREFPixelShader        () { return m_pixelShader; }

		auto GetVALPrimitiveTopologyType() const { return m_primitiveTopologyType; }

		auto GetVALDSVFormat() const { return m_dsvFormat; }

		auto GetVALSampleMask() const { return m_sampleMask; }

		auto GetVALUseRootSignatureTag() const { return m_useRootSignatureTag; }

	private:

		D3D12_SHADER_BYTECODE FetchShaderByteCode(const Shader& a_shader) const;

		TypeAlias::ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;

		std::shared_ptr<Shader> m_amplificationShader = nullptr;
		std::shared_ptr<Shader> m_pixelShader         = nullptr;

		D3D12_RASTERIZER_DESC    m_rasterizerDesc   = {};
		D3D12_BLEND_DESC         m_blendDesc        = {};
		D3D12_DEPTH_STENCIL_DESC m_depthStencilDesc = {};
		DXGI_SAMPLE_DESC	     m_sampleDesc       = {};

		D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitiveTopologyType = Constant::k_defaultPrimitiveTopologyType;

		DXGI_FORMAT m_dsvFormat = Constant::k_defaultDSVFormat;

		UINT m_sampleMask = Constant::k_initialSampleMask;

		Shader m_meshShader = {};

		Converter::PipelineStateJsonConverter m_pipelineStateJsonConverter = {};

		TypeAlias::TypeTag m_useRootSignatureTag = Constant::k_invalidTypeTag;

		std::vector<DXGI_FORMAT> m_rtvFormatList = {};
	};
}