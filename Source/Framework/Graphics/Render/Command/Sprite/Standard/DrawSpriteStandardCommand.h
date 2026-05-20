#pragma once

namespace FWK::Graphics
{
	class DrawSpriteStandardCommand final : public DrawRequestCommandBase<Struct::SpriteStandardDrawCommand>
	{
	public:

		 DrawSpriteStandardCommand()          = default;
		~DrawSpriteStandardCommand() override = default;

		void PostCreateSetup(Renderer& a_renderer) override;

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override;

	private:

		bool SetupCBSpritePass(const std::weak_ptr<RootSignature>& a_rootSignature,
							   const Renderer&					   a_renderer,
							   const DirectCommandList&			   a_directCommandList,
							   const UploadBuffer&				   a_spritePassUploadBuffer,
								     std::uint8_t* const		   a_spritePassMappedData) const;

		bool SetupCBSpriteDraw(const std::weak_ptr<RootSignature>&         a_rootSignature,
							   const Struct::SpriteStandardDrawCommand&	   a_spriteStandardDrawCommand,
							   const std::weak_ptr<Struct::TextureRecord>& a_textureRecord,
							   const DirectCommandList&			           a_directCommandList,
							   const UploadBuffer&				           a_spriteDrawUploadBuffer,
							   const std::size_t&				           a_spriteDrawCommandIndex,
									 std::uint8_t* const		           a_spriteDrawMappedData) const;

		static constexpr float k_defaultNearClip = 0.0F;
		static constexpr float k_defaultFarClip  = 1.0F;

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		static constexpr std::size_t k_cbSpritePassIndex = 0ULL;

		// ※注意 テンプレートクラスをDrawCommandBaseは使っているのでそのDrawCommandBaseを継承している
		// 基底クラスとして設定する
		FWK_DEFINE_TYPE_INFO(DrawSpriteStandardCommand, DrawCommandBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::ShaderFactoryDrawCommand, FWK::Graphics::DrawSpriteStandardCommand);