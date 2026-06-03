#include "DrawRequestPerObjectBase.h"

std::weak_ptr<FWK::Graphics::RootSignature> FWK::Graphics::DrawRequestPerObjectBase::SetGraphicsPipelineStateAndFetchRootSignature(Renderer& a_renderer, const TypeAlias::TypeTag a_pipelineStateTag) const
{
	const auto& l_pipelineStateWeak = a_renderer.FindVALPipelineState(a_pipelineStateTag);
	const auto& l_pipelineState     = l_pipelineStateWeak.lock       ();

	FWK_ASSERT_RETURN_VALUE_IF(!l_pipelineState, "指定されたPipelineStateTagに対応するPipelineStateが無効です。", {})

	// PipelineStateに設定されているRootSignatureTagを取得する。
	// 派生クラス側はRootSignatureTagを直接指定しなくてよい
	const auto l_rootSignatureTag = l_pipelineState->GetVALUseRootSignatureTag();

	const auto& l_rootSignatureWeak = a_renderer.FindVALRootSignature(l_rootSignatureTag);

	auto& l_directCommandList = a_renderer.GetMutableREFDirectCommandList();

	l_directCommandList.SetupRootSignature(l_rootSignatureWeak);
	l_directCommandList.SetupPipelineState(l_pipelineStateWeak);

	// 定数バッファ設定に使う
	return l_rootSignatureWeak;
}

void FWK::Graphics::DrawRequestPerObjectBase::TransitionTextureToPixelShaderResource(const DirectCommandList& a_directCommandList, Graphics::TextureRecord& a_textureRecord) const
{
	if (const auto l_currentState = a_textureRecord.GetVALCurrentState();
		l_currentState == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) 
	{
		return; 
	}

	const auto& l_textureResource = a_textureRecord.GetREFGPUResource().m_resource;

	FWK_ASSERT_RETURN_IF(!l_textureResource, "テクスチャリソースが無効になっており、状態遷移を行えませんでした。")

	// PixelShaderからSRVとして読むため、現在の状態からPIXEL_SHADER_RESOURCEへ遷移する
	a_directCommandList.TransitionResource(a_textureRecord.GetVALCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, *l_textureResource.Get());

	a_textureRecord.SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

bool FWK::Graphics::DrawRequestPerObjectBase::TransitionTextureToPixelShaderResource(const std::shared_ptr<Texture>& a_texture, 
																					 const DirectCommandList&		 a_directCommandList, 
																					 const TextureSystem&			 a_textureSystem, 
																					 const Enum::DefaultTextureType  a_defaultTextureType) const
{
	if (a_texture)
	{
		const auto& l_textureRecord = a_texture->GetREFTextureRecord().lock();

		if (l_textureRecord)
		{
			// MaterialやSpriteが持っているTextureをPixelShaderから読める状態へ遷移する
			TransitionTextureToPixelShaderResource(a_directCommandList, *l_textureRecord);

			return true;
		}
	}

	// Textureが無い、またはTextureRecordが無効の場合はDefaultTextureを使う。
	const auto& l_defaultTextureRecord = a_textureSystem.FindVALDefaultTextureRecord(a_defaultTextureType).lock();

	FWK_ASSERT_RETURN_VALUE_IF(!l_defaultTextureRecord, "DefaultTextureが取得できないため、Textureの状態遷移に失敗しました。", false)

	TransitionTextureToPixelShaderResource(a_directCommandList, *l_defaultTextureRecord);

	return true;
}

FWK::TypeAlias::StorageID FWK::Graphics::DrawRequestPerObjectBase::FetchVALTextureSRVStorageID(const std::shared_ptr<Texture>& a_texture, const TextureSystem& a_textureSystem, const Enum::DefaultTextureType a_defaultTextureType) const 
{
	if (a_texture)
	{
		// テクスチャIDが無効でなければその値を返す
		const auto& l_textureRecord = a_texture->GetREFTextureRecord().lock();

		if (l_textureRecord &&
			l_textureRecord->GetVALSRVStorageID() != Constant::k_invalidStorageID)
		{
			return l_textureRecord->GetVALSRVStorageID();
		}
	}

	// SRVがTextureRecordから取得できない場合デフォルトテクスチャのSRVStorageIDを返す
	const auto& l_defaultTextureRecord = a_textureSystem.FindVALDefaultTextureRecord(a_defaultTextureType).lock();

	FWK_ASSERT_RETURN_VALUE_IF(!l_defaultTextureRecord, "DefaultTextureが取得できませんでした。", Constant::k_invalidStorageID)

	const auto l_defaultTextureSRVStorageID = l_defaultTextureRecord->GetVALSRVStorageID();

	FWK_ASSERT_RETURN_VALUE_IF(l_defaultTextureSRVStorageID == Constant::k_invalidStorageID, "DefaultTextureRecordのSRVStorageIDが無効です。", Constant::k_invalidStorageID)

	return l_defaultTextureSRVStorageID;
}
FWK::TypeAlias::StorageID FWK::Graphics::DrawRequestPerObjectBase::FetchVALTextureSRVStorageID(const std::shared_ptr<TextureRecord>& a_textureRecord, const TextureSystem& a_textureSystem, const Enum::DefaultTextureType a_defaultTextureType) const 
{
	if (a_textureRecord)
	{
		// テクスチャIDが無効でなければその値を返す
		if (const auto l_textureStorageID = a_textureRecord->GetVALSRVStorageID();
			l_textureStorageID != Constant::k_invalidStorageID) 
		{
			return l_textureStorageID; 
		}
	}

	// SRVがTextureRecordから取得できない場合デフォルトテクスチャのSRVStorageIDを返す
	const auto& l_defaultTextureRecord = a_textureSystem.FindVALDefaultTextureRecord(a_defaultTextureType).lock();

	FWK_ASSERT_RETURN_VALUE_IF(!l_defaultTextureRecord, "DefaultTextureが取得できませんでした。", Constant::k_invalidStorageID)

	const auto l_defaultTextureSRVStorageID = l_defaultTextureRecord->GetVALSRVStorageID();

	FWK_ASSERT_RETURN_VALUE_IF(l_defaultTextureSRVStorageID == Constant::k_invalidStorageID, "DefaultTextureRecordのSRVStorageIDが無効です。", Constant::k_invalidStorageID)

	return l_defaultTextureSRVStorageID;
}