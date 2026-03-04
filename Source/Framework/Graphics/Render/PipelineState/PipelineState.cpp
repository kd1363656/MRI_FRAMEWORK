#include "PipelineState.h"

FWK::Graphics::PipelineState::PipelineState(const Device& a_device, const ShaderCompiler& a_shaderCompiler) :
	k_device                    (a_device),
	k_shaderCompiler            (a_shaderCompiler),
	m_amplificationShader       (nullptr),
	m_pixelShader               (nullptr),
	m_pipelineState             (nullptr),
	m_meshShader                (k_shaderCompiler),
	m_pipelineStateJsonConverter(*this)
{}
FWK::Graphics::PipelineState::~PipelineState() = default;

void FWK::Graphics::PipelineState::Init()
{
	m_pipelineState.Reset();

	m_meshShader.SetFilePath("Asset/Shader/Simple/SimpleMS.hlsl");
	m_meshShader.SetEntryPointName("main");
	m_meshShader.SetShaderModelName("ms_6_5");

	if (!m_pixelShader)
	{
		m_pixelShader = std::make_unique<Shader>(k_shaderCompiler);
	}

	m_pixelShader->SetFilePath("Asset/Shader/Simple/SimplePS.hlsl");
	m_pixelShader->SetEntryPointName("main");
	m_pixelShader->SetShaderModelName("ps_6_5");
}

bool FWK::Graphics::PipelineState::Create(const RootSignature& a_rootSignature)
{
	const auto& l_device = k_device.GetDevice().Get();

	if (!l_device)
	{
		assert(false && "デバイスが作製されておらず、パイプラインステートの作製が出来ませんでした。");
		return false;
	}

	const auto& l_rootSignature = a_rootSignature.GetRootSignature();
	
	if (!l_rootSignature)
	{
		assert(false && "ルートシグネチャが作成されておらず、パイプラインステートの作成が出来ませんでした。");
		return false;
	}

	// 増幅、メッシュ、ピクセルシェーダーを読み込む
	if (m_amplificationShader)
	{
		m_amplificationShader->CompileFromFile();
	}

	m_meshShader.CompileFromFile();

	if (m_pixelShader)
	{
		m_pixelShader->CompileFromFile();
	}
	
	auto l_pipelineStateDesc = D3DX12_MESH_SHADER_PIPELINE_STATE_DESC();

	// // 増幅、メッシュ、ピクセルシェーダーのセット
	if (m_amplificationShader)
	{
		l_pipelineStateDesc.AS = FetchShaderByteCode(*m_amplificationShader);
	}

	l_pipelineStateDesc.MS = FetchShaderByteCode(m_meshShader);

	if (m_pixelShader)
	{
		l_pipelineStateDesc.PS = FetchShaderByteCode(*m_pixelShader);
	}

	// ルートシグネチャのセット
	l_pipelineStateDesc.pRootSignature = l_rootSignature.Get();

	// ラスタライザーステートのセット
	l_pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// ブレンドステートのセット
	l_pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	auto l_depthDesc = D3D12_DEPTH_STENCIL_DESC();

	l_depthDesc.DepthEnable   = FALSE;
	l_depthDesc.StencilEnable = FALSE;
	
	// デプスステンシルパラメータをセット
	l_pipelineStateDesc.DepthStencilState = l_depthDesc;

	l_pipelineStateDesc.SampleMask = UINT_MAX;

	l_pipelineStateDesc.NumRenderTargets = 1;
	l_pipelineStateDesc.RTVFormats[0]    = DXGI_FORMAT_R8G8B8A8_UNORM;

	// サンプルデスクをセット
	l_pipelineStateDesc.SampleDesc            = DefaultSampleDesc();
	l_pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	auto l_psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(l_pipelineStateDesc);

	auto l_streamDesc = D3D12_PIPELINE_STATE_STREAM_DESC();

	l_streamDesc.pPipelineStateSubobjectStream = &l_psoStream;
	l_streamDesc.SizeInBytes                   = sizeof(l_psoStream);

	auto l_hr = l_device->CreatePipelineState(&l_streamDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "パイプラインステート作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::PipelineState::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_pipelineStateJsonConverter.Deserialize(a_rootJson);
}
nlohmann::json FWK::Graphics::PipelineState::Serialize()
{
	return m_pipelineStateJsonConverter.Serialize();
}

void FWK::Graphics::PipelineState::EnsureShader(std::unique_ptr<Shader>& a_shader) const
{
	// 既にインスタンス化されていたら"return"
	if (a_shader) { return; }

	a_shader = std::make_unique<Shader>(k_shaderCompiler);
}
D3D12_SHADER_BYTECODE FWK::Graphics::PipelineState::FetchShaderByteCode(const Shader& a_shader) const
{
	const auto& l_blob = a_shader.GetBlob();

	if (!l_blob)
	{
		assert(false && "シェーダーバイトコードの取得に失敗しました。");
		return D3D12_SHADER_BYTECODE();
	}

	return D3D12_SHADER_BYTECODE(l_blob->GetBufferPointer(), l_blob->GetBufferSize());
}