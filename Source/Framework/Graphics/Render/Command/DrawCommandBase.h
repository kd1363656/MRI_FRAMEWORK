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
		bool SetupConstantBuffer(const RootSignature&	   a_rootSignature,
								 const DirectCommandList&  a_directCommandList,
								 const ConstantBufferType& a_constantBuffer,
									   UploadBuffer&	   a_uploadBuffer,
									   std::uint8_t* const a_mappedData) const
		{
			if (!a_mappedData)
			{
				assert(false && "定数バッファのMap済みアドレスが無効なため、定数バッファの設定に失敗しました。");
				return false;
			}

			// 定数バッファは256バイトアライメントでなければならない
			const auto& l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(ConstantBufferType), Constant::k_constantBufferAlignment);

			const auto& l_currentBufferIndex = a_uploadBuffer.AllocateCurrentBufferIndex();

			// 定数バッファの位置を現在のインデックス分進める
			const auto l_constantBufferOffset = l_currentBufferIndex * l_constantBufferAlignedSize;

			std::memcpy(a_mappedData + l_constantBufferOffset, &a_constantBuffer, sizeof(ConstantBufferType));

			const auto l_gpuVirtualAddress = a_uploadBuffer.FetchVALGPUVirtualAddress() + l_constantBufferOffset;

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);
			// SetupConstantBufferView内でRootParameterTagからルートパラメータ番号を取得し、
			// 指定したRootParameterへUploadBuffer上の定数バッファを結びつける
			a_directCommandList.SetupConstantBufferView<RootParameterTagType>(l_gpuVirtualAddress, a_rootSignature);

			return true;
		}

		// 定数バッファの上書きを許可
		template <Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		bool SetupConstantBuffer(const RootSignature&	   a_rootSignature,
								 const DirectCommandList&  a_directCommandList,
								 const ConstantBufferType& a_constantBuffer,
								 const std::size_t&		   a_constantBufferIndex,
									   UploadBuffer&	   a_uploadBuffer,
									   std::uint8_t* const a_mappedData) const
		{
			if (!a_mappedData)
			{
				assert(false && "定数バッファのMap済みアドレスが無効なため、定数バッファの設定に失敗しました。");
				return false;
			}

			// 定数バッファは256バイトアライメントでなければならない
			const auto& l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(ConstantBufferType), Constant::k_constantBufferAlignment);

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

		template < Concept::IsDerivedConstantBufferUploaderBaseConcept ConstantBufferUploaderType, Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename CBType>
		bool SetupCommonPassConstantBuffer(const RootSignature&	     a_rootSignature,
										   const DirectCommandList&  a_directCommandList,
										   const FrameResource&		 a_frameResource,
										   const CBType&			 a_constantBuffer,
										   const std::size_t&		 a_constantBufferIndex)
		{
			auto l_constantBufferUploader = a_frameResource.FindPTRConstantBuffer<ConstantBufferUploaderType>().lock();

			if (!l_constantBuffer)
			{
				assert(false && "共通パス定数バッファが取得できないため、描画処理に失敗しました。");
				return false;
			}

			auto&		l_constantUploadBuffer = l_constantBuffer->GetMutableREFUploadConstantBuffer();
			auto* const l_mappedData		   = l_constantUploadBuffer.Map						    ();

			if (!l_mappedData)
			{
				assert(false && "共通パス定数バッファのMapに失敗したため、描画処理に失敗しました。");
				return false;
			}

			// 共通パスの定数バッファのセットは一回のみでよい
			if (!SetupConstantBuffer<RootParameterTagType>(a_rootSignature,
														   a_directCommandList,
														   a_constantBuffer,
														   a_constantBufferIndex,
														   l_constantUploadBuffer,
														   l_mappedData))
			{
				assert(false && "共通パス定数バッファのGPU送信命令に失敗したため、描画処理に失敗しました。");
				return false;
			}

			l_constantUploadBuffer.UnMap();

			return true;
		}

		void SetupPipelineStateAndRootSignature(const Renderer& a_renderer, const TypeAlias::TypeTag a_typeTag);

		void SetupGraphicsPipelineStateToCommandList(Renderer& a_renderer) const;

		void TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Struct::TextureRecord& a_textureRecord) const;

		static constexpr auto& GetREFCommonPassIndex() { return k_cbCommonPassIndex; }

		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountX() { return k_defaultDispatchMeshThreadGroupCountX; }
		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountY() { return k_defaultDispatchMeshThreadGroupCountY; }
		static constexpr UINT GetVALDefaultDispatchMeshThreadGroupCountZ() { return k_defaultDispatchMeshThreadGroupCountZ; }

	private:

		static constexpr std::size_t k_cbCommonPassIndex = 0ULL;

		static constexpr UINT k_defaultDispatchMeshThreadGroupCountX = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountY = 1U;
		static constexpr UINT k_defaultDispatchMeshThreadGroupCountZ = 1U;

		std::weak_ptr<RootSignature> m_rootSignature = {};
		std::weak_ptr<PipelineState> m_pipelineState = {};

		FWK_DEFINE_TYPE_INFO_ROOT(DrawCommandBase);
	};
}