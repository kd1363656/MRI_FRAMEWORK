#pragma once

namespace FWK::Graphics
{
	template <typename Type>
	class DrawCommandBase : public IDrawCommand
	{
	private:

		struct GraphicsPipelineSetupResult final
		{
			const RootSignature* m_rootSignature = nullptr;
			const PipelineState* m_pipelineState = nullptr;
		};

	public:

		 DrawCommandBase()		    = default;
		~DrawCommandBase() override = default;

		void BeginFrame() override
		{
			m_drawCommandList.clear();
		}

		void RequestDraw(const Type& a_drawCommand)
		{
			m_drawCommandList.emplace_back(a_drawCommand);
		}

		const auto& GetREFDrawCommandList() const { return m_drawCommandList; }

	protected:

		template <Concept::IsDerivedPipelineStateTagBaseConcept PipelineStateType>
		GraphicsPipelineSetupResult SetupGraphicsPipelineStateByTag(Renderer& a_renderer) const
		{
			auto* l_pipelineState = a_renderer.FindPTRPipelineState(Utility::Tag::GetTag<PipelineStateType>());

			if (!l_pipelineState) 
			{
				assert(false && "使用するパイプラインステートが作成されておらず、描画を開始できませんでした。");
				return {};
			}

			// パイプラインステートが使用するルートシグネチャを取得
			auto* l_rootSignature = a_renderer.FindPTRRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

			if (!l_rootSignature)
			{
				assert(false && "使用するルートシグネチャが作成されておらず、描画を開始できませんでした。");
				return {};
			}

			auto& l_directCommandList = a_renderer.GetMutableREFDirectCommandList();

			// ルートシグネチャをセット
			l_directCommandList.SetupRootSignature(l_rootSignature);

			// パイプラインステートをセット
			l_directCommandList.SetupPipelineState(l_pipelineState);
			
			// 使用したルートシグネチャ、パイプラインステートを外部に渡す
			return { l_rootSignature, l_pipelineState };
		}

		void TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Struct::TextureRecord& a_textureRecord)
		{
			if (a_textureRecord.m_currentState == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) { return; }

			// PixelsShaderからSRVとして読むため、現在の状態からPIXEL_SHADER_RESOURCEへ遷移する
			a_directCommandList.TransitionResource(a_textureRecord.m_textureResource,
												   a_textureRecord.m_currentState,
												   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			a_textureRecord.m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		}

		template <Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		bool SetupConstantBuffer(const DirectCommandList &       a_directCommandList,
								 const UploadBuffer		 &	     a_uploadBuffer,
								 const ConstantBufferType&       a_constantBuffer,
								 const RootSignature     *	     a_rootSignature,
								 const std::size_t		         a_constantBufferIndex,
									   std::uint8_t      * const a_mappedData) const
		{
			if (!a_rootSignature)
			{
				assert(false && "ルートシグネチャが無効なため、定数バッファの設定に失敗しました。");
				return false;
			}

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

	private:

		std::vector<Type> m_drawCommandList = {};
	};
}