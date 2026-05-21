#pragma once

namespace FWK::Graphics
{
	class DrawStaticModelLitStandardCommand final : public DrawStaticModelStandardCommandBase<Tag::ModelLitStandardPipelineStateTag>
	{
	public:

		 DrawStaticModelLitStandardCommand()		  = default;
		~DrawStaticModelLitStandardCommand() override = default;

		FWK_DEFINE_TYPE_INFO(DrawStaticModelLitStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawStaticModelLitStandardCommand);