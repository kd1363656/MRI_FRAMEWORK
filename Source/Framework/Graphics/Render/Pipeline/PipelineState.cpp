#include "PipelineState.h"

void FWK::Graphics::PipelineState::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
}

bool FWK::Graphics::PipelineState::Create(const Device& a_device, const ShaderCompiler& a_shaderCompiler, const Renderer& a_renderer)
{
	const auto& l_device = a_device.GetREFDevice().Get();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、パイプラインステートの作成が出来ませんでした。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::PipelineState::Serialize() const
{
	return nlohmann::json();
}

void FWK::Graphics::PipelineState::EnsureShader(std::shared_ptr<Shader>& a_shader) const
{
	// 既にインスタンス化されていたら"return"
	if (a_shader) { return; }

	a_shader = std::make_shared<Shader>();
}