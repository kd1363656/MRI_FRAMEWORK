#pragma once

namespace FWK::Graphics
{
	class DrawRequestPassBase
	{
	public:

				 DrawRequestPassBase() = default;
		virtual ~DrawRequestPassBase() = default;

		virtual bool SetupCommonPassConstantBuffer(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource) = 0;

	protected:

		// 定数バッファの上書きを許可
		template <Concept::IsDerivedConstantBufferUploaderBaseConcept ConstantBufferUploaderType, Concept::IsDerivedRootParameterTagBaseConcept RootParameterTagType, typename ConstantBufferType>
		bool SetupConstantBuffer(const RootSignature&	   a_rootSignature,
								 const DirectCommandList&  a_directCommandList,
								 const FrameResource&	   a_frameResource,
								 const ConstantBufferType& a_constantBuffer)
		{
			const auto& l_constantBufferUploader = a_frameResource.FindPTRConstantBufferUploader<ConstantBufferUploaderType>().lock();

			FWK_ASSERT_RETURN_VALUE_IF(!l_constantBufferUploader, "共通パス定数バッファが取得できないため、描画処理に失敗しました。", false)

			const auto& l_gpuVirtualAddress = l_constantBufferUploader.WriteCommonPass(a_constantBuffer);

			// SetGraphicsRootConstantBufferView(ルートパラメータ番号、
			//									 CBVとして参照させるGPU仮想アドレス);
			// SetupConstantBufferView内でRootParameterTagからルートパラメータ番号を取得し、
			// 指定したRootParameterへUploadBuffer上の定数バッファを結びつける
			a_directCommandList.SetupConstantBufferView<RootParameterTagType>(l_gpuVirtualAddress, a_rootSignature);

			return true;
		}

		FWK_DEFINE_TYPE_INFO_ROOT(DrawRequestPassBase)
	};
}