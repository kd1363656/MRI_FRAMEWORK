#pragma once

namespace FWK::Graphics
{
	class DrawSpriteStandardCommand final : public IDrawCommand
	{
	public:

		 DrawSpriteStandardCommand()          = default;
		~DrawSpriteStandardCommand() override = default;

		void BeginFrame() override;

		void Draw(const Renderer& a_renderer, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, TextureSystem& a_textureSystem) override;

		void RequestSpriteStandardDraw(const Struct::SpriteDrawCommand& a_spriteStandardDraw);

	private:

		std::vector<Struct::SpriteDrawCommand> m_spriteStandardCommandList = {};

		FWK_DEFINE_TYPE_INFO(DrawSpriteStandardCommand, IDrawCommand);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawSpriteStandardCommand);