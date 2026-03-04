#pragma once

namespace FWK::Graphics
{
	class PipelineState;
}

namespace FWK::Converter
{
	class PipelineStateJsonConverter final : public JsonConverterBase
	{
	public:

		explicit PipelineStateJsonConverter(Graphics::PipelineState& a_pipelineState);
		~PipelineStateJsonConverter        ()										  override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()                                 override;

	private:

		Graphics::PipelineState& m_pipelineState;
	};
}