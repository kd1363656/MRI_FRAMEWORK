#pragma once

namespace FWK::Graphics
{
	class DrawStaticModelUnLitStandardCommand final : public DrawStaticModelStandardCommandBase<Tag::ModelUnLitStandardPipelineStateTag>
	{
	public:

		 DrawStaticModelUnLitStandardCommand()		    = default;
		~DrawStaticModelUnLitStandardCommand() override = default;
		
		void Draw(Renderer& a_renderer) override;

		FWK_DEFINE_TYPE_INFO(DrawStaticModelUnLitStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawStaticModelUnLitStandardCommand)