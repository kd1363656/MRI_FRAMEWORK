#pragma once

namespace FWK::Graphics
{
	class PipelineState final
	{
	public:

		explicit PipelineState(const Device& a_device, const ShaderCompiler& a_shaderCompiler);
		~PipelineState        ();

		void Init  ();
		bool Create(const RootSignature& a_rootSignature);

		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize();

		void EnsureShader(std::unique_ptr<Shader>& a_shader) const;

		const auto& GetPipelineState() const { return m_pipelineState; }
		
		auto& GetWorkAmplificationShader() { return m_amplificationShader; }
		auto& GetWorkMeshShader         () { return m_meshShader; }
		auto& GetWorkPixelShader        () { return m_pixelShader; }

	private:

		D3D12_SHADER_BYTECODE FetchShaderByteCode(const Shader& a_shader) const;

		const Device&         k_device;
		const ShaderCompiler& k_shaderCompiler;

		std::unique_ptr<Shader> m_amplificationShader;
		std::unique_ptr<Shader> m_pixelShader;

		ComPtr<ID3D12PipelineState> m_pipelineState;

		Shader m_meshShader;

		Converter::PipelineStateJsonConverter m_pipelineStateJsonConverter;
	};
}