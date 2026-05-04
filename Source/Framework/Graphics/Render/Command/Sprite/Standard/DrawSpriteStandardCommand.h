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

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		// ※注意 テンプレートクラスをDrawCommandBaseは使っているのでそのDrawCommandBaseを継承している
		// 基底クラスとして設定する
		FWK_DEFINE_TYPE_INFO(DrawSpriteStandardCommand, IDrawCommand);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawSpriteStandardCommand);