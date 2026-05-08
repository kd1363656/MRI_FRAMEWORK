#pragma once

namespace FWK::Graphics
{
	class DrawSpriteStandardCommand final : public DrawCommandBase<Struct::SpriteDrawCommand>
	{
	public:

		 DrawSpriteStandardCommand()          = default;
		~DrawSpriteStandardCommand() override = default;

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer, TextureSystem& a_textureSystem) override;

	private:

		bool SetupCBSpritePass(const Renderer         &		  a_renderer,
							   const DirectCommandList&       a_directCommandList,
							   const UploadBuffer     &	      a_spritePassUploadBuffer,
							   const RootSignature    *	      a_rootSignature,
								     std::uint8_t     * const a_spritePassMappedData);

		bool SetupCBSpriteDraw(const Struct::SpriteDrawCommand&				a_spriteDrawCommand,
							   const DirectCommandList		  &				a_directCommandList,
							   const UploadBuffer			  &				a_spriteDrawUploadBuffer,
							   const RootSignature			  *			    a_rootSignature,
							   const std::size_t							a_spriteDrawCommandIndex,
									 std::uint8_t			  * const		a_spriteDrawMappedData) const;

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		static constexpr float k_defaultNearClip = 0.0F;
		static constexpr float k_defaultFarClip  = 1.0F;
		
		static constexpr std::size_t k_initialSpriteDrawCommandListIndex = 0ULL;
		static constexpr std::size_t k_cbSpritePassIndex			     = 0ULL;

		// ※注意 テンプレートクラスをDrawCommandBaseは使っているのでそのDrawCommandBaseを継承している
		// 基底クラスとして設定する
		FWK_DEFINE_TYPE_INFO(DrawSpriteStandardCommand, IDrawCommand);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawSpriteStandardCommand);