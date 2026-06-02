#include "StaticModelStandardPerObjectDrawRequestBase.h"

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::BeginFrame()
{
	// 参照が途切れているstd::weak_ptrを削除する
	m_forwardPerObjectDataList.BeginFrame();
}

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::AddForwardDrawRequestData(const std::shared_ptr<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestData)
{
	m_forwardPerObjectDataList.AddDrawRequestPerObject(a_staticModelStandardPerObjectDrawRequestData);
}

void FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::SetupModelMeshConstantBuffer(const RootSignature&																   a_rootSignature, 
																							  const DirectCommandList&															   a_directCommandList,
																							  const FrameResource&																   a_frameResource,
																							  const DrawRequestPerObjectList<Struct::StaticModelStandardPerObjectDrawRequestData>& a_staticModelStandardPerObjectDrawRequestDataList,
																							  const TextureSystem&																   a_textureSystem)
{
	for (const auto& l_drawRequest : a_staticModelStandardPerObjectDrawRequestDataList.GetREFDrawRequestPerObjectRecordList())
	{
		const auto& l_drawRequestPerObject = l_drawRequest.m_drawRequestPerObject.lock();

		if (!l_drawRequestPerObject) { continue; }

		const auto& l_staticModelRecord = l_drawRequestPerObject->m_staticModelRecord.lock();

		FWK_ASSERT_RETURN_IF(!l_staticModelRecord, "StaticModelRecordのポインタが無効です。")

		const auto& l_modelData = l_staticModelRecord->GetREFModelData();

		for(const auto& l_modelMesh : l_modelData.m_modelMeshList)
		{
			// メッシュ単位ごとに実行
			const auto& l_modelMeshletData         = l_modelMesh.m_modelMeshletData;
			const auto& l_modelMeshRuntimeData     = l_modelMesh.m_modelMeshRuntimeData;
			const auto& l_modelMaterialRuntimeData = l_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData;

			FWK_ASSERT_RETURN_IF(l_modelMeshletData.m_meshletList.size() == Constant::k_emptyMeshletCount, "Meshletが存在しないため、StaticModelのPerObject定数バッファを設定できませんでした")

			Struct::CBModelPerObject l_cbModelPerObject = {};

			// モデル1体ごとの行列
			l_cbModelPerObject.m_worldMatrix = l_drawRequestPerObject->m_worldMatrix;

			// MeshShaderで参照するStructuredBufferのSRV番号
			l_cbModelPerObject.m_vertexBufferSRVIndex            = l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID;
			l_cbModelPerObject.m_meshletBufferSRVIndex           = l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID;
			l_cbModelPerObject.m_uniqueVertexIndexBufferSRVIndex = l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID;
			l_cbModelPerObject.m_primitiveIndexBufferSRVIndex    = l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID;

			FWK_ASSERT_RETURN_IF(l_cbModelPerObject.m_vertexBufferSRVIndex		      == Constant::k_invalidStorageID, "VertexBufferのSRVStorageIDが無効です。")
			FWK_ASSERT_RETURN_IF(l_cbModelPerObject.m_meshletBufferSRVIndex		      == Constant::k_invalidStorageID, "MeshletBufferのSRVStorageIDが無効です。")
			FWK_ASSERT_RETURN_IF(l_cbModelPerObject.m_uniqueVertexIndexBufferSRVIndex == Constant::k_invalidStorageID, "UniqueVertexIndexBufferのSRVStorageIDが無効です。")
			FWK_ASSERT_RETURN_IF(l_cbModelPerObject.m_primitiveIndexBufferSRVIndex    == Constant::k_invalidStorageID, "PrimitiveIndexBufferのSRVStorageIDが無効です。")
				
			// BaseColorTextureのSRV番号。
			// Textureがない場合はDefaultTextureを使う
			l_cbModelPerObject.m_baseColorTextureSRVIndex = FetchVALTextureSRVStorageID(l_modelMaterialRuntimeData.m_baseColorTexture, a_textureSystem,				   Enum::DefaultTextureType::BaseColor);
			FWK_ASSERT_RETURN_IF												       (l_cbModelPerObject.m_baseColorTextureSRVIndex == Constant::k_invalidStorageID, "BaseColorTextureのSRVStorageIDが無効です。")

			l_cbModelPerObject.m_normalTextureSRVIndex = FetchVALTextureSRVStorageID(l_modelMaterialRuntimeData.m_normalTexture, a_textureSystem,				 Enum::DefaultTextureType::Normal);
			FWK_ASSERT_RETURN_IF												    (l_cbModelPerObject.m_normalTextureSRVIndex == Constant::k_invalidStorageID, "NormalTextureのSRVStorageIDが無効です。")

			SetupPerObjectConstantBuffer<ModelPerObjectConstantBufferUploader, Tag::RootParameterCBModelPerObjectTag>(a_rootSignature, 
																													  a_directCommandList,
																													  a_frameResource,
																													  l_cbModelPerObject);	
		}
	}
}

bool FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::TransitionMaterialTexture(const DirectCommandList& a_directCommandList, const TextureSystem& a_textureSystem, const Struct::ModelMesh& a_modelMesh) const
{
	const auto& l_modelMaterialRuntimeData = a_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData;

	const bool l_isBaseColorTextureSuccess = TransitionTextureToPixelShaderResource(l_modelMaterialRuntimeData.m_baseColorTexture,
																					a_directCommandList,
																					a_textureSystem,
																					Enum::DefaultTextureType::BaseColor);

	FWK_ASSERT_RETURN_VALUE_IF(!l_isBaseColorTextureSuccess, "BaseColorTextureの状態遷移に失敗しました。", false)

	const bool l_isNormalTextureSuccess = TransitionTextureToPixelShaderResource(l_modelMaterialRuntimeData.m_normalTexture,
																				 a_directCommandList,
																				 a_textureSystem,
																				 Enum::DefaultTextureType::Normal);


	FWK_ASSERT_RETURN_VALUE_IF(!l_isNormalTextureSuccess, "NormalTextureの状態遷移に失敗しました。", false)

	return true;
}

bool FWK::Graphics::StaticModelStandardPerObjectDrawRequestBase::DispatchModelMesh(const DirectCommandList& a_directCommandList, const Struct::ModelMesh& a_modelMesh) const
{
	const auto l_meshletCount = static_cast<UINT>(a_modelMesh.m_modelMeshletData.m_meshletList.size());

	FWK_ASSERT_RETURN_VALUE_IF(l_meshletCount == Constant::k_emptyMeshletCount, "Meshletが存在しないため、StaticModelを描画できませんでした。", false)

	a_directCommandList.DispatchMesh(l_meshletCount, GetVALDefaultDispatchMeshThreadGroupCountY(), GetVALDefaultDispatchMeshThreadGroupCountZ());

	return true;
}