#pragma once

namespace FWK::Graphics
{
	class SwapChain;
	class RenderArea;
	class RootSignature;
	class PipelineState;
}

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		 DirectCommandList();
		~DirectCommandList() override;

		void Reset(const CommandAllocatorBase& a_commandAllocator) override;

		void TransitionResource            (const TypeAlias::ComPtr<ID3D12Resource2>& a_resource,  const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState) const;
		void TransitionRenderTargetResource(const SwapChain&						  a_swapChain, const D3D12_RESOURCE_STATES a_beforeState, const D3D12_RESOURCE_STATES a_afterState) const;

		void SetupBackBuffer(const SwapChain& a_swapChain, const RTVDescriptorHeap& a_rtvDescriptorHeap) const;

		void SetupRenderArea    (const RenderArea&		   a_renderArea)     const;
		void SetupRootSignature (const RootSignature*	   a_rootSignature);
		void SetupPipelineState (const PipelineState*	   a_pipelineState);
		void SetupDescriptorHeap(const DescriptorHeapBase& a_descriptorHeap) const;

		template <Concept::IsDerivedRootParameterTagBaseConcept Type>
		void SetupDescriptorTable(const DescriptorHeapBase& a_descriptorHeap, const RootSignature* a_rootSignature, const UINT a_srvIndex) const
		{
			const auto& l_directCommandList = GetREFCommandList();

			if (!l_directCommandList)
			{
				assert(false && "ダイレクトコマンドリストが作成されておらず、ディスクリプタテーブル設定が出来ませんでした。");
				return;
			}

			if (!a_rootSignature)
			{
				assert(false && "ルートシグネチャが作成されておらず、ディスクリプタテーブル設定ができませんでした。");
				return;
			}

			if (a_srvIndex == Constant::k_invalidDescriptorHeapIndex)
			{
				assert(false && "SRVインデックスが無効なため、ディスクリプタテーブル設定ができませんでした。");
				return;
			}

			// ルートシグネチャで定義したディスクリプタテーブルへ、
			// 実際のディスクリプタビューのGPUハンドルを結び付ける
			// SetGraphicsRootDescriptorTable(ルートパラメータ番号、
			//								  ディスクリプタテーブル先頭GPUハンドル);

			const auto l_rootParameterIndex = a_rootSignature->FindVALRootParameterIndex(Utility::Tag::GetTag<Type>());

			if (l_rootParameterIndex == Constant::k_invalidRootParameterIndex)
			{
				assert(false && "パラメータインデックスが無効なため、ディスクリプタテーブル設定ができませんでした。");
				return;
			}

			const auto& l_gpuHandle = a_descriptorHeap.FetchVALShaderVisibleGPUHandle(a_srvIndex);

			if (l_gpuHandle.ptr == k_invalidGPUDescriptorHandle)
			{
				assert(false && "GPUディスクリプタハンドルが無効なため、ディスクリプタテーブル設定ができませんでした。");
				return;
			}

			l_directCommandList->SetGraphicsRootDescriptorTable(l_rootParameterIndex, l_gpuHandle);
		}

		template <Concept::IsDerivedRootParameterTagBaseConcept Type>
		void SetupConstantBufferView(const D3D12_GPU_VIRTUAL_ADDRESS& a_gpuVirtualAddress, const RootSignature* a_rootSignature) const
		{
			const auto& l_directCommandList = GetREFCommandList();

			if (!l_directCommandList)
			{
				assert(false && "ルートシグネチャが作成されておらず、定数バッファビュー設定が出来ませんでした。");
				return;
			}

			if (!a_rootSignature)
			{
				assert(false && "ルートシグネチャが作成されておらず、定数バッファビューが設定できませんでした。");
				return;
			}

			const auto l_rootParameterIndex = a_rootSignature->FindVALRootParameterIndex(Utility::Tag::GetTag<Type>());

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

		static constexpr float k_clearColor[] = 
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

		const RootSignature* m_currentRootSignature = nullptr;
		const PipelineState* m_currentPipelineState = nullptr;
	};
}