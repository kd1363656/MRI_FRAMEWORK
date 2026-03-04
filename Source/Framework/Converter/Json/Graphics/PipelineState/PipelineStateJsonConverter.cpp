#include "PipelineStateJsonConverter.h"

FWK::Converter::PipelineStateJsonConverter::PipelineStateJsonConverter(Graphics::PipelineState& a_pipelineState) : 
	m_pipelineState(a_pipelineState)
{}
FWK::Converter::PipelineStateJsonConverter::~PipelineStateJsonConverter() = default;

void FWK::Converter::PipelineStateJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	auto& l_amplificationShader = m_pipelineState.GetWorkAmplificationShader();
	auto& l_meshShader          = m_pipelineState.GetWorkMeshShader         ();
	auto& l_pixelShader         = m_pipelineState.GetWorkPixelShader        ();

	if (a_rootJson.contains("AmplificationShader"))
	{
		// "json"で保存されていたということは生成されていたということなので
		// インスタンス化されていなければインスタンス化して読み取る
		m_pipelineState.EnsureShader(l_amplificationShader);

		l_amplificationShader->Deserialize(a_rootJson["AmplificationShader"]);
	}

	if (a_rootJson.contains("MeshShader"))
	{
		l_meshShader.Deserialize(a_rootJson["MeshShader"]);
	}

	if (a_rootJson.contains("PixelShader"))
	{
		// "json"で保存されていたということは生成されていたということなので
		// インスタンス化されていなければインスタンス化して読み取る
		m_pipelineState.EnsureShader(l_pixelShader);

		l_pixelShader->Deserialize(a_rootJson["PixelShader"]);
	}
}
nlohmann::json FWK::Converter::PipelineStateJsonConverter::Serialize()
{
	auto l_rootJson  = nlohmann::json{};
	
	const auto& l_amplificationShader = m_pipelineState.GetWorkAmplificationShader();
	auto&		l_meshShader          = m_pipelineState.GetWorkMeshShader         ();
	const auto& l_pixelShader         = m_pipelineState.GetWorkPixelShader        ();

	if (l_amplificationShader)
	{
		l_rootJson["AmplificationShader"] = l_amplificationShader->Serialize();
	}

	l_rootJson["MeshShader"] = l_meshShader.Serialize();

	if (l_pixelShader)
	{
		l_rootJson["PixelShader"] = l_pixelShader->Serialize();
	}

	return l_rootJson;
}