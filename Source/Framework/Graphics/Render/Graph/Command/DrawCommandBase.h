#pragma once

namespace FWK::Graphics
{
	class Renderer;
}

namespace FWK::Graphics
{
	// Rendererクラスでそのフレームで必要な座標だったりサイズだったりを持つためのクラス
	class DrawCommandBase
	{
	public:

				 DrawCommandBase() = default;
		virtual ~DrawCommandBase() = default;

		virtual void BeginFrame		()					   = 0;
		virtual void PostCreateSetup(Renderer& a_renderer) = 0;

				void SetupDraw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) const;
		virtual void Draw     (		 Renderer&							a_renderer) = 0;

		const auto& GetVALRootSignature() const { return m_rootSignature; }
		const auto& GetVALPipelineState() const { return m_pipelineState; }

	protected:

		// 定数バッファの上書き禁止
		template <Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		void SetupPerDrawConstantBuffer(const RootSignature&			  a_rootSignature,
								        const DirectCommandList&		  a_directCommandList,
								        const ConstantBufferType&		  a_constantBuffer,
								          	  ConstantBufferUploaderBase& a_constantBufferUploaderBase) const
		{
			const auto& l_gpuVirtualAddress = a_constantBufferUploaderBase.Write(a_constantBuffer);

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);
			// SetupConstantBufferView内でRootParameterTagからルートパラメータ番号を取得し、
			// 指定したRootParameterへUploadBuffer上の定数バッファを結びつける
			a_directCommandList.SetupConstantBufferView<RootParameterTagType>(l_gpuVirtualAddress, a_rootSignature);
		}

		// 定数バッファの上書きを許可
		template <Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		bool SetupCommonConstantBuffer(const RootSignature&			     a_rootSignature,
									   const DirectCommandList&		     a_directCommandList,
									   const ConstantBufferType&		 a_constantBuffer,
											 ConstantBufferUploaderBase& a_constantBufferUploader) const
		{
			const auto l_gpuVirtualAddress = a_constantBufferUploader.WriteCommonPass(a_constantBuffer);

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);
			// SetupConstantBufferView内でRootParameterTagからルートパラメータ番号を取得し、
			// 指定したRootParameterへUploadBuffer上の定数バッファを結びつける
			a_directCommandList.SetupConstantBufferView<RootParameterTagType>(l_gpuVirtualAddress, a_rootSignature);

			return true;
		}

		template <Concept::IsDerivedConstantBufferUploaderBaseConcept ConstantBufferUploaderType, Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename CBType>
		bool SetupCommonPassConstantBuffer(const RootSignature&	    a_rootSignature,
										   const DirectCommandList& a_directCommandList,
										   const FrameResource&	    a_frameResource,
										   const CBType&			a_constantBuffer) const
		{
			auto l_constantBufferUploader = a_frameResource.FindPTRConstantBufferUploader<ConstantBufferUploaderType>().lock();
		
			FWK_ASSERT_RETURN_VALUE_IF(!l_constantBufferUploader, "共通パス定数バッファが取得できないため、描画処理に失敗しました。", false)
		
			return SetupCommonConstantBuffer<RootParameterTagType>(a_rootSignature,
																   a_directCommandList,
																   a_constantBuffer,
																   *l_constantBufferUploader);
		}

		void SetupPipelineStateAndRootSignature(const Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag);

		void SetupGraphicsPipelineStateToCommandList(Renderer& a_renderer) const;

		void TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Graphics::TextureRecord& a_textureRecord) const;

		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountX() { return k_defaultDispatchMeshThreadGroupCountX; }
		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountY() { return k_defaultDispatchMeshThreadGroupCountY; }
		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountZ() { return k_defaultDispatchMeshThreadGroupCountZ; }

	private:

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		std::weak_ptr<RootSignature> m_rootSignature = {};
		std::weak_ptr<PipelineState> m_pipelineState = {};

		FWK_DEFINE_TYPE_INFO_ROOT(DrawCommandBase)
	};
}