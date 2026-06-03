#pragma once

namespace FWK::Graphics
{
	class StaticModelStandardPerObjectDrawRequestUnLit final : public StaticModelStandardPerObjectDrawRequestBase
	{
	public:

		 StaticModelStandardPerObjectDrawRequestUnLit()			 = default;
		~StaticModelStandardPerObjectDrawRequestUnLit() override = default;

		void RequestForwardDraw(const TextureSystem& a_textureSystem, Renderer& a_renderer) override;

	private:

		FWK_DEFINE_TYPE_INFO(StaticModelStandardPerObjectDrawRequestUnLit, StaticModelStandardPerObjectDrawRequestBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryDrawRequestPerObject, FWK::Graphics::StaticModelStandardPerObjectDrawRequestUnLit)