#pragma once

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

		virtual void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) = 0;

		const auto& GetVALRootSignature() const { return m_rootSignature; }
		const auto& GetVALPipelineState() const { return m_pipelineState; }

	protected:

		template <Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		bool SetupConstantBuffer(const RootSignature&	   a_rootSignature,
								 const DirectCommandList&  a_directCommandList,
								 const UploadBuffer&	   a_uploadBuffer,
								 const ConstantBufferType& a_constantBuffer,
								 const std::size_t&		   a_constantBufferIndex,
									   std::uint8_t* const a_mappedData) const
		{
			if (!a_mappedData)
			{
				assert(false && "定数バッファのMap済みアドレスが無効なため、定数バッファの設定に失敗しました。");
				return false;
			}

			// 定数バッファは256バイトアライメントでなければならない
			const auto l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(ConstantBufferType), Constant::k_constantBufferAlignment);

			// 定数バッファの位置を現在のインデックス分進める
			const auto l_constantBufferOffset = a_constantBufferIndex * l_constantBufferAlignedSize;

			std::memcpy(a_mappedData + l_constantBufferOffset, &a_constantBuffer, sizeof(ConstantBufferType));

			const auto l_gpuVirtualAddress = a_uploadBuffer.FetchVALGPUVirtualAddress() + l_constantBufferOffset;

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);
			// SetupConstantBufferView内でRootParameterTagからルートパラメータ番号を取得し、
			// 指定したRootParameterへUploadBuffer上の定数バッファを結びつける
			a_directCommandList.SetupConstantBufferView<RootParameterTagType>(l_gpuVirtualAddress, a_rootSignature);

			return true;
		}

		void SetupPipelineStateAndRootSignature(const Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag);

		void SetupGraphicsPipelineStateToCommandList(Renderer& a_renderer) const;

		void TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Struct::TextureRecord& a_textureRecord) const;

		bool SetCBCamera(const std::weak_ptr<Camera>& a_camera,
						 const RootSignature&		  a_rootSignature,
						 const DirectCommandList&	  a_directCommandList,
						 const UploadBuffer&		  a_cameraUploadBuffer,
							   std::uint8_t* const	  a_cameraMappedData) const;

	private:

		static constexpr std::size_t k_cameraConstantBufferIndex = 0ULL;

		std::weak_ptr<RootSignature> m_rootSignature = {};
		std::weak_ptr<PipelineState> m_pipelineState = {};

		FWK_DEFINE_TYPE_INFO_ROOT(DrawCommandBase);
	};
}