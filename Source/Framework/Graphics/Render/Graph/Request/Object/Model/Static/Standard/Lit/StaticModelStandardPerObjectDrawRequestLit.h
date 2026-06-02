#pragma once

namespace FWK::Graphics
{
	class StaticModelStandardPerObjectDrawRequestLit final : public StaticModelStandardPerObjectDrawRequestBase
	{
	public:

		 StaticModelStandardPerObjectDrawRequestLit()		   = default;
		~StaticModelStandardPerObjectDrawRequestLit() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void RequestForwardDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer) override;

		FWK_DEFINE_TYPE_INFO(StaticModelStandardPerObjectDrawRequestLit, StaticModelStandardPerObjectDrawRequestBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryDrawRequestPerObject, FWK::Graphics::StaticModelStandardPerObjectDrawRequestLit)