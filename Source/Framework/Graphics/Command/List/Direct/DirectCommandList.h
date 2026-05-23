#pragma once

namespace FWK::Graphics
{
	class SwapChain;
	class RenderArea;
	class RootSignature;
	class PipelineState;
	class DepthStencilTexture;
}

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		 DirectCommandList();
		~DirectCommandList() override;

		void Reset(const CommandAllocatorBase& a_commandAllocator) override;

		void TransitionResource            (const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState,		ID3D12Resource2&	  a_resource) const;
		void TransitionRenderTargetResource(const SwapChain&			a_swapChain,   const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState) const;

		void SetupBackBuffer(const SwapChain&		    a_swapChain, 
							 const RTVDescriptorHeap&   a_rtvDescriptorHeap,
							 const DSVDescriptorHeap&   a_dsvDescriptorHeap,
						     const DepthStencilTexture& a_depthStencilTexture) const;

		void SetupRenderArea    (const RenderArea&					 a_renderArea)     const;
		void SetupRootSignature (const std::weak_ptr<RootSignature>& a_rootSignature);
		void SetupPipelineState (const std::weak_ptr<PipelineState>& a_pipelineState);
		void SetupDescriptorHeap(const DescriptorHeapBase&			 a_descriptorHeap) const;

		template <Concept::IsDerivedRootParameterTagBaseConcept Type>
		void SetupConstantBufferView(const D3D12_GPU_VIRTUAL_ADDRESS& a_gpuVirtualAddress, const RootSignature& a_rootSignature) const
		{
			const auto& l_directCommandList = GetREFCommandList();

			if (!l_directCommandList)
			{
				assert(false && "ルートシグネチャが作成されておらず、定数バッファビュー設定が出来ませんでした。");
				return;
			}

			const auto l_rootParameterIndex = a_rootSignature.FindVALRootParameterIndex(Utility::Tag::GetTag<Type>());

			if (l_rootParameterIndex == Constant::k_invalidRootParameterIndex)
			{
				assert(false && "パラメータインデックスが無効なため、定数バッファビュー設定ができませんでした。");
				return;
			}

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);

			// RootSignature側でD3D12_ROOT_PARAMETER_TYPE_CBVにした場所へ、
			// UploadBuffer上の定数バッファ位置を直接結びつける
			l_directCommandList->SetGraphicsRootConstantBufferView(l_rootParameterIndex, a_gpuVirtualAddress);
		}

		void DispatchMesh(const UINT a_threadCountGroupX, const UINT a_threadCountGroupY, const UINT a_threadCountGroupZ) const;

	private:

		void ClearCurrentRootSignatureAndPipelineStateCache();

		template <typename Type>
		bool IsSameWeakOwner(const std::weak_ptr<Type>& a_left, const std::weak_ptr<Type>& a_right) const
		{
			return !a_left.owner_before (a_right) &&
				   !a_right.owner_before(a_left);
		}

		static constexpr TypeAlias::Math::Color k_clearColor = 
		{
			1.0F,
			0.80F,
			1.0F,
			1.0F
		};

		static constexpr UINT64 k_invalidGPUDescriptorHandle = 0ULL;

		static constexpr UINT k_sendBarrierNUM = 1U;

		static constexpr UINT k_executeRenderTargetNUM = 1U;
		static constexpr UINT k_executeClearRectNUM    = 0U;

		static constexpr UINT k_setViewportNUM       = 1U;
		static constexpr UINT k_setScissorRectNUM    = 1U;
		static constexpr UINT k_setDescriptorHeapNUM = 1U;

		std::weak_ptr<RootSignature> m_currentRootSignature = {};
		std::weak_ptr<PipelineState> m_currentPipelineState = {};
	};
}