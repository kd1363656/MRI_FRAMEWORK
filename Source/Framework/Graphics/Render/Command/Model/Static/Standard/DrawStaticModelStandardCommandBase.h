#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedPipelineStateTagBaseConcept Type>
	class DrawStaticModelStandardCommandBase : public DrawRequestCommandBase<Struct::StaticModelStandardDrawCommand, Struct::StaticModelStandardPassConstant>
	{
	public:

		 DrawStaticModelStandardCommandBase()		   = default;
		~DrawStaticModelStandardCommandBase() override = default;

		void PostCreateSetup(Renderer& a_renderer) override
		{
			SetupPipelineStateAndRootSignature(a_renderer, Utility::Tag::GetTag<Type>());
		}

		bool SetupCommonPassConstantBuffer(Renderer& a_renderer)
		{
			const auto& l_rootSignature = GetVALRootSignature().lock();

			if (!l_rootSignature)
			{
				assert(false && "使用しようとしたルートシグネチャが無効なため、StaticModel描画処理に失敗しました。");
				return false;
			}

			const auto& l_currentFrameResource = a_renderer.GetREFCurrentFrameResource().lock();

			if (!l_currentFrameResource)
			{
				assert(false && "現在のフレームリソースの取得に失敗しました。");
				return false;
			}

			const auto& l_staticModelStandardPassConstant = GetPassConstant().lock();
			
			if (!l_staticModelStandardPassConstant) { return false; }

			const auto& l_cbCamera = l_staticModelStandardPassConstant->m_camera.lock();

			if (!l_cbCamera) { return false; }

			const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

			// もし共通定数バッファの設定に失敗したらマップを解除
			if (!DrawCommandBase::SetupCommonPassConstantBuffer<CameraConstantBufferUploader, Tag::RootParameterCBCameraTag>(*l_rootSignature,
																															 l_directCommandList,
																															 *l_currentFrameResource,
																															 l_cbCamera->CreateCBCamera(),
																															 GetREFCommonPassIndex()))
			{
				assert(false && "共通パスの定数バッファが設定できず、描画処理に失敗しました。");
				return false;
			}

			return true;
		}

		bool SetupCBModelObject(const RootSignature& a_rootSignature, const DirectCommandList& a_directCommandList, const FrameResource& a_frameResource) const
		{
			// モデルオブジェクトマップ用定数バッファの取得
			auto l_modelObjectConstantBuffer = a_frameResource.FindPTRConstantBuffer<ModelObjectConstantBufferUploader>().lock();

			if (!l_modelObjectConstantBuffer)
			{
				assert(false && "ModelObject用定数バッファが取得できないため、StaticModel描画処理に失敗しました。");
				return false;
			}

				  auto&		  l_uploadBuffer					   = l_modelObjectConstantBuffer->GetMutableREFUploadBuffer();
				  auto* const l_mappedData						   = l_uploadBuffer.Map									   ();
			const auto&		  l_staticModelStandardDrawCommandList = GetREFDrawCommandList								   ();

			for (const auto& l_staticModelStandardDrawCommand : l_staticModelStandardDrawCommandList)
			{
				const auto& l_drawCommand = l_staticModelStandardDrawCommand.lock();

				if (!l_drawCommand) { continue; }

				const auto& l_staticModelRecord = l_drawCommand->m_staticModelRecord.lock();

				if (!l_staticModelRecord) { continue; }

				const auto& l_modelMeshList = l_staticModelRecord->m_modelData.m_modelMeshList;

				for (const auto& l_modelMesh : l_modelMeshList)
				{
					const auto& l_modelMeshletData         = l_modelMesh.m_modelMeshletData;
					const auto& l_modelMeshRuntimeData     = l_modelMesh.m_modelMeshRuntimeData;
					const auto& l_modelMaterialRuntimeData = l_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData;

					if (l_modelMeshletData.m_meshletList.size() == Constant::k_emptyMeshletCount) { continue; }

					// モデル定数のセット
					if (!l_modelMaterialRuntimeData.m_baseColorTexture)
					{
						assert(false && "BaseColorTextureが無効なため、StaticModel描画処理に失敗しました。");
						return false;
					}
					
					if (!l_modelMaterialRuntimeData.m_normalTexture)
					{
						assert(false && "NormalTextureが無効なため、StaticModel描画処理に失敗しました。");
						return false;
					}
					const auto& l_baseColorTextureRecord = l_modelMaterialRuntimeData.m_baseColorTexture->GetREFTextureRecord().lock();
					
					if (!l_baseColorTextureRecord)
					{
						assert(false && "BaseColorTexture用TextureRecordが無効なため、StaticModel描画処理に失敗しました。");
						return false;
					}
					
					const auto& l_normalTextureRecord = l_modelMaterialRuntimeData.m_normalTexture->GetREFTextureRecord().lock();
					
					if (!l_normalTextureRecord)
					{
						assert(false && "NormalTexture用TextureRecordが無効なため、StaticModel描画処理に失敗しました。");
						return false;
					}
					
					if (!ValidateModelMeshStructuredBufferSRV(l_modelMeshRuntimeData))
					{
						assert(false && "ModelMesh用StructuredBufferのSRVStorageIDが無効なため、StaticModel描画処理に失敗しました。");
						return false;
					}

					Struct::CBModelObject l_cbModelObject = {};

					l_cbModelObject.m_worldMatrix		              = l_drawCommand->m_worldMatrix;
					l_cbModelObject.m_baseColorTextureSRVIndex        = l_baseColorTextureRecord->m_srvStorageID;
					l_cbModelObject.m_normalTextureSRVIndex           = l_normalTextureRecord->m_srvStorageID;
					l_cbModelObject.m_vertexBufferSRVIndex            = l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID;
					l_cbModelObject.m_meshletBufferSRVIndex           = l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID;
					l_cbModelObject.m_uniqueVertexIndexBufferSRVIndex = l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID;
					l_cbModelObject.m_primitiveIndexBufferSRVIndex    = l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID;

					if(!SetupConstantBuffer<Tag::RootParameterCBModelObjectTag>(a_rootSignature, 
																				a_directCommandList,
																				l_cbModelObject,
																				l_uploadBuffer,
																				l_mappedData))
					{
						continue;
					}

					a_directCommandList.DispatchMesh(static_cast<UINT>(l_modelMeshletData.m_meshletList.size()), GetVALDefaultDispatchMeshThreadGroupCountY(), GetVALDefaultDispatchMeshThreadGroupCountZ());
				}
			}

			return true;
		}

	private:

		bool ValidateModelMeshStructuredBufferSRV(const Struct::ModelMeshRuntimeData& a_modelMeshRuntimeData) const
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
	};
}