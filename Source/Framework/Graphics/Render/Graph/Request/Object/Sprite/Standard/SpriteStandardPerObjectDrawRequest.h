#pragma once

namespace FWK::Graphics
{
	class Renderer;
}

namespace FWK::Graphics
{
	class SpriteStandardPerObjectDrawRequest final : public DrawRequestPerObjectBase
	{
	public:

		 SpriteStandardPerObjectDrawRequest()		   = default;
		~SpriteStandardPerObjectDrawRequest() override = default;

		void BeginFrame() override;
		
		void PostCreateSetup(Renderer& a_renderer) override;

		void RequestDraw(Renderer& a_renderer);

		void AddDrawRequestPerObject(const std::shared_ptr<Struct::SpriteStandardPerObjectDrawRequestData>& a_drawRequestData);
		
	private:

		DrawRequestPerObjectList<Struct::SpriteStandardPerObjectDrawRequestData> m_drawRequestPerObjectList = {};

		FWK_DEFINE_TYPE_INFO(SpriteStandardPerObjectDrawRequest, DrawRequestPerObjectBase)
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryDrawRequestPerObject, FWK::Graphics::SpriteStandardPerObjectDrawRequest)