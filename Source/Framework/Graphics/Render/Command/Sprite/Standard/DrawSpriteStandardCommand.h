#pragma once

namespace FWK::Graphics
{
	class DrawSpriteStandardCommand final : public DrawRequestCommandBase<Struct::SpriteStandardDrawCommand>
	{
	public:

		 DrawSpriteStandardCommand()          = default;
		~DrawSpriteStandardCommand() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Draw(Renderer& a_renderer) override;

	private:

		bool SetupCBSpriteObject(const RootSignature&					   a_rootSignature,
								 const DirectCommandList&			       a_directCommandList,
								 const Struct::TextureRecord&			   a_textureRecord,
								 const Struct::SpriteStandardDrawCommand&  a_spriteStandardDrawCommand,
									   UploadBuffer&				       a_spriteDrawUploadBuffer,
									   std::uint8_t* const		           a_spriteDrawMappedData) const;

		static constexpr float k_defaultNearClip = 0.0F;
		static constexpr float k_defaultFarClip  = 1.0F;

		// ※注意 テンプレートクラスをDrawCommandBaseは使っているのでそのDrawCommandBaseを継承している
		// 基底クラスとして設定する
		FWK_DEFINE_TYPE_INFO(DrawSpriteStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawSpriteStandardCommand);