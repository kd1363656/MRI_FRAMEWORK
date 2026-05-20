#include "DrawStaticModelUnLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelUnLitStandardCommand::PostCreateSetup(Renderer& a_renderer)
{
	SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetTag<Tag::ModelUnLitStandardPipelineStateTag>());
}
void FWK::Graphics::DrawStaticModelUnLitStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer)
{
	const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

	// MeshShaderからSRVを読むため、ShaderVisibleのSRVDescriptorHeapを設定する
	l_directCommandList.SetupDescriptorHeap(a_srvDescriptorPool.GetREFDescriptorHeap());

	// StaticModel用ルートシグネチャとパイプラインステートをセット
	SetupGraphicsPipelineStateToCommandList(a_renderer);

	const auto& l_rootSignature = GetVALRootSignature();

	if (l_rootSignature.expired())
	{
		assert(false && "使用しようとしたルートシグネチャが無効なため、StaticModel描画処理に失敗しました。");
		return;
	}

	const auto& l_currentFrameResource = a_renderer.FetchVALCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		assert(false && "現在のフレームリソースの取得に失敗しました。");
		return;
	}

	auto l_cameraConstantBuffer		 = l_currentFrameResource->FindPTRConstantBuffer<CameraConstantBuffer>      ().lock();
	auto l_modelObjectConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<ModelObjectConstantBuffer> ().lock();
	
	if (!l_cameraConstantBuffer)
	{
		assert(false && "Camera用定数バッファが取得できないため、StaticModel描画処理に失敗しました。");
		return;
	}

	if (!l_modelObjectConstantBuffer)
	{
		assert(false && "ModelObject用定数バッファが取得できないため、StaticModel描画処理に失敗しました。");
		return;
	}

	const auto& l_cameraUploadBuffer	  = l_cameraConstantBuffer->GetREFUploadConstantBuffer	   ();
	const auto& l_modelObjectUploadBuffer = l_modelObjectConstantBuffer->GetREFUploadConstantBuffer();
	
	auto* const l_cameraMappedData	    = l_cameraUploadBuffer.Map	   ();
	auto* const l_modelObjectMappedData = l_modelObjectUploadBuffer.Map();
	
	if (!l_cameraMappedData)
	{
		assert(false && "Camera用定数バッファのMapに失敗したため、StaticModel描画処理に失敗しました。");
		return;
	}

	if (!l_modelObjectMappedData)
	{
		assert(false && "ModelObject用定数バッファのMapに失敗したため、StaticModel描画処理に失敗しました。");
		l_cameraUploadBuffer.UnMap();
		return;
	}

	const auto& l_staticModelDrawCommandList = GetREFDrawCommandList();

	std::size_t l_modelObjectIndex = 0ULL;
	
	for (const auto& l_staticModelDrawCommand : l_staticModelDrawCommandList)
	{
		const auto& l_staticModelRecord = l_staticModelDrawCommand.m_staticModelRecord.lock();

		if (!l_staticModelRecord) { continue; }

		// カメラ情報をセット
		if (!SetCBCamera(l_rootSignature,
						 l_staticModelDrawCommand.m_camera,
						 l_directCommandList,
						 l_cameraUploadBuffer,
						 l_cameraMappedData))
		{
			continue;
		}

		const auto& l_modelMeshList = l_staticModelRecord->m_modelData.m_modelMeshList;

		for (const auto& l_modelMesh : l_modelMeshList)
		{
			const auto& l_modelMeshletData     = l_modelMesh.m_modelMeshletData;
			const auto& l_modelMeshRuntimeData = l_modelMesh.m_modelMeshRuntimeData;

			if (l_modelMeshletData.m_meshletList.size() == Constant::k_emptyMeshletCount) { continue; }

			// モデル定数のセット
			if (!SetupCBModelObject(l_rootSignature,
									l_staticModelDrawCommand,
									l_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData,
									l_modelMeshRuntimeData,
								    l_directCommandList,
								    l_modelObjectUploadBuffer,
									l_modelObjectIndex,
									l_modelObjectMappedData))
			{
				continue;
			}

			l_directCommandList.DispatchMesh(static_cast<UINT>(l_modelMeshletData.m_meshletList.size()), k_defaultDispatchMeshThreadGroupCountY, k_defaultDispatchMeshThreadGroupCountZ);

			++l_modelObjectIndex;
		}
	}

	l_modelObjectUploadBuffer.UnMap();
	l_cameraUploadBuffer.UnMap	   ();
}

bool FWK::Graphics::DrawStaticModelUnLitStandardCommand::SetupCBModelObject(const std::weak_ptr<RootSignature>&				   a_rootSignature,
																			const Struct::StaticModelUnLitStandardDrawCommand& a_staticModelUnLitStandardDrawCommand, 
																		    const Struct::ModelMaterialRuntimeData&			   a_modelMaterialRuntimeData,
																		    const Struct::ModelMeshRuntimeData&			       a_modelMeshRuntimeData,
																			const DirectCommandList&						   a_directCommandList, 
																			const UploadBuffer&								   a_modelObjectUploadBuffer, 
																			const std::size_t&								   a_modelObjectIndex, 
																				  std::uint8_t* const						   a_modelObjectMappedData) const
{
	if (!a_modelMaterialRuntimeData.m_baseColorTexture)
	{
		assert(false && "BaseColorTextureが無効なため、StaticModel描画処理に失敗しました。");
		return false;
	}

	const auto& l_textureRecord = a_modelMaterialRuntimeData.m_baseColorTexture->GetREFTextureRecord().lock();

	if (!l_textureRecord)
	{
		assert(false && "BaseColorTexture用TextureRecordが無効なため、StaticModel描画処理に失敗しました。");
		return false;
	}

	if (!ValidateModelMeshStructuredBufferSRV(a_modelMeshRuntimeData))
	{
		assert(false && "ModelMesh用StructuredBufferのSRVStorageIDが無効なため、StaticModel描画処理に失敗しました。");
		return false;
	}

	Struct::CBModelObject l_cbModelObject = {};

	l_cbModelObject.m_worldMatrix		           = a_staticModelUnLitStandardDrawCommand.m_worldMatrix;
	l_cbModelObject.m_baseColorTextureIndex        = l_textureRecord->m_srvStorageID;
	l_cbModelObject.m_vertexBufferIndex            = a_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID;
	l_cbModelObject.m_meshletBufferIndex           = a_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID;
	l_cbModelObject.m_uniqueVertexIndexBufferIndex = a_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID;
	l_cbModelObject.m_primitiveIndexBufferIndex    = a_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID;

	return SetupConstantBuffer<Tag::RootParameterCBModelObjectTag>(a_rootSignature,
																   a_directCommandList,
																   a_modelObjectUploadBuffer,
																   l_cbModelObject,
																   a_modelObjectIndex,
																   a_modelObjectMappedData);
}

bool FWK::Graphics::DrawStaticModelUnLitStandardCommand::ValidateModelMeshStructuredBufferSRV(const Struct::ModelMeshRuntimeData& a_modelMeshRuntimeData) const
{
	if (a_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID		    == Constant::k_invalidStorageID ||
		a_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID           == Constant::k_invalidStorageID ||
		a_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID == Constant::k_invalidStorageID ||
		a_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID	== Constant::k_invalidStorageID)
	{
		return false;
	}

	return true;
}