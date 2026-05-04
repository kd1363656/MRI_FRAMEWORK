#pragma once

namespace FWK::Graphics
{
	class DrawSpriteStandardCommand final : public DrawCommandBase<Struct::SpriteDrawCommand>
	{
	public:

		 DrawSpriteStandardCommand()          = default;
		~DrawSpriteStandardCommand() override = default;

		void Draw(const Renderer& a_renderer, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, TextureSystem& a_textureSystem) override;

	private:

		// ※注意 テンプレートクラスをDrawCommandBaseは使っているのでそのDrawCommandBaseを継承している
		// 基底クラスとして設定する
		FWK_DEFINE_TYPE_INFO(DrawSpriteStandardCommand, IDrawCommand);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawSpriteStandardCommand);