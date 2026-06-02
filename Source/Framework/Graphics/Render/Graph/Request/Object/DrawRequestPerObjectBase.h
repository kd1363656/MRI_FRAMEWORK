#pragma once

namespace FWK::Graphics
{
	class Renderer;
}

namespace FWK::Graphics	
{
	class DrawRequestPerObjectBase
	{
	public:

			     DrawRequestPerObjectBase() = default;
		virtual ~DrawRequestPerObjectBase() = default;

		virtual void BeginFrame() = 0;

	protected:

		// 定数バッファの上書き禁止
		template <Concept::IsDerivedConstantBufferUploaderBaseConcept ConstantBufferUploaderType, Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		bool SetupPerObjectConstantBuffer(const RootSignature&	    a_rootSignature,
										  const DirectCommandList&  a_directCommandList,
										  const FrameResource&	    a_frameResource,
										  const ConstantBufferType& a_constantBuffer)
		{
			auto l_constantBufferUploader = a_frameResource.FindPTRConstantBufferUploader<ConstantBufferUploaderType>().lock();

			FWK_ASSERT_RETURN_VALUE_IF(!l_constantBufferUploader, "PerObject定数バッファが取得できないため、描画処理に失敗しました。", false)

			const auto& l_gpuVirtualAddress = l_constantBufferUploader->Write(a_constantBuffer);

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);
			// SetupConstantBufferView内でRootParameterTagからルートパラメータ番号を取得し、
			// 指定したRootParameterへUploadBuffer上の定数バッファを結びつける
			a_directCommandList.SetupConstantBufferView<RootParameterTagType>(l_gpuVirtualAddress, a_rootSignature);

			return true;
		}

		void SetupPipelineStateAndRootSignature(const Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag);

		void SetupGraphicsPipelineStateToCommandList(Renderer& a_renderer) const;

		void TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Graphics::TextureRecord& a_textureRecord) const;

		bool TransitionTextureToPixelShaderResource(const std::shared_ptr<Texture>& a_texture,
												    const DirectCommandList&		a_directCommandList,
													const TextureSystem&			a_textureSystem,
													const Enum::DefaultTextureType  a_defaultTextureType) const;

		TypeAlias::StorageID FetchVALTextureSRVStorageID(const std::shared_ptr<Texture>& a_texture, const TextureSystem& a_textureSystem, const Enum::DefaultTextureType a_defaultTextureType) const;

		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountX() { return k_defaultDispatchMeshThreadGroupCountX; }
		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountY() { return k_defaultDispatchMeshThreadGroupCountY; }
		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountZ() { return k_defaultDispatchMeshThreadGroupCountZ; }

		const auto& GetVALRootSignature() const { return m_rootSignature; }
		const auto& GetVALPipelineState() const { return m_pipelineState; }

	private:

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		std::weak_ptr<RootSignature> m_rootSignature = {};
		std::weak_ptr<PipelineState> m_pipelineState = {};

		FWK_DEFINE_TYPE_INFO_ROOT(DrawRequestPerObjectBase)
	};
}