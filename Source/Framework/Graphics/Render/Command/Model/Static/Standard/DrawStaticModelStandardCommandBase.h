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

		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override
		{
			const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

			const auto& l_rootSignature = GetVALRootSignature().lock();

			if (!l_rootSignature)
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

			const auto& l_lightSystem = a_renderer.GetREFLightSystem();

			auto l_cameraConstantBuffer		 = l_currentFrameResource->FindPTRConstantBuffer<CameraConstantBuffer>      ().lock();
			auto l_modelObjectConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<ModelObjectConstantBuffer> ().lock();
			auto l_lightSystemConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<LightConstantBuffer>       ().lock();

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

			if (!l_lightSystemConstantBuffer)
			{
				assert(false && "Light用定数バッファが取得できないため、StaticModelLit描画処理に失敗しました。");
				return;
			}

			const auto& l_cameraUploadBuffer	  = l_cameraConstantBuffer->GetREFUploadConstantBuffer	   ();
			const auto& l_modelObjectUploadBuffer = l_modelObjectConstantBuffer->GetREFUploadConstantBuffer();
			const auto& l_lightSystemUploadBuffer = l_lightSystemConstantBuffer->GetREFUploadConstantBuffer();

			auto* const l_cameraMappedData	    = l_cameraUploadBuffer.Map	   ();
			auto* const l_modelObjectMappedData = l_modelObjectUploadBuffer.Map();
			auto* const l_lightSystemMappedData = l_lightSystemUploadBuffer.Map();
			
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

			if (!l_lightSystemMappedData)
			{
				assert(false && "Light用定数バッファが取得できないため、StaticModelLit描画処理に失敗しました。");
				l_cameraUploadBuffer.UnMap	   ();
				l_modelObjectUploadBuffer.UnMap();
				return;
			}

			// セットされていなければreturn
			if (!GetPTRPassConstant()) { return; }

			// カメラ情報を1回だけセット
			if (const auto& l_camera = GetPTRPassConstant()->m_camera.lock();
				!SetCBCamera(*l_camera,
					         *l_rootSignature,
					         l_directCommandList,
					         l_cameraUploadBuffer,
					         l_cameraMappedData))
			{
				l_cameraUploadBuffer.UnMap	   ();
				l_modelObjectUploadBuffer.UnMap();
				l_lightSystemUploadBuffer.UnMap();
				return;
			}

			// ライト情報を一回だけセット
			if (!SetCBLight(l_lightSystem,
							*l_rootSignature,
							l_directCommandList,
							l_lightSystemUploadBuffer,
							l_lightSystemMappedData))
			{
				l_cameraUploadBuffer.UnMap	   ();
				l_modelObjectUploadBuffer.UnMap();
				l_lightSystemUploadBuffer.UnMap();
				return;
			}

			const auto& l_staticModelDrawCommandList = GetREFDrawCommandList();

			std::size_t l_modelObjectIndex = 0ULL;
			
			for (const auto& l_staticModelDrawCommand : l_staticModelDrawCommandList)
			{
				const auto& l_staticModelRecord = l_staticModelDrawCommand.m_staticModelRecord.lock();

				if (!l_staticModelRecord) { continue; }

				const auto& l_modelMeshList = l_staticModelRecord->m_modelData.m_modelMeshList;

				for (const auto& l_modelMesh : l_modelMeshList)
				{
					const auto& l_modelMeshletData     = l_modelMesh.m_modelMeshletData;
					const auto& l_modelMeshRuntimeData = l_modelMesh.m_modelMeshRuntimeData;

					if (l_modelMeshletData.m_meshletList.size() == Constant::k_emptyMeshletCount) { continue; }

					// モデル定数のセット
					if (!SetupCBModelObject(*l_rootSignature,
											l_directCommandList,
										    l_modelObjectUploadBuffer,
											l_staticModelDrawCommand,
											l_modelMesh.m_modelMaterial.m_modelMaterialRuntimeData,
											l_modelMeshRuntimeData,
											l_modelObjectIndex,
											l_modelObjectMappedData))
					{
						continue;
					}

					l_directCommandList.DispatchMesh(static_cast<UINT>(l_modelMeshletData.m_meshletList.size()), GetVALDefaultDispatchMeshThreadGroupCountY(), GetVALDefaultDispatchMeshThreadGroupCountZ());

					++l_modelObjectIndex;
				}
			}

			l_modelObjectUploadBuffer.UnMap();
			l_cameraUploadBuffer.UnMap	   ();
			l_lightSystemUploadBuffer.UnMap();
		}

	private:

		bool SetupCBModelObject(const RootSignature&						  a_rootSignature,
								const DirectCommandList&		              a_directCommandList,
								const UploadBuffer&					          a_modelObjectUploadBuffer,
								const Struct::StaticModelStandardDrawCommand& a_staticModelStandardDrawCommand,
								const Struct::ModelMaterialRuntimeData&		  a_modelMaterialRuntimeData,
								const Struct::ModelMeshRuntimeData&			  a_modelMeshRuntimeData,
								const std::size_t&							  a_modelObjectIndex,
									  std::uint8_t* const			          a_modelObjectMappedData) const
		{
			if (!a_modelMaterialRuntimeData.m_baseColorTexture)
			{
				assert(false && "BaseColorTextureが無効なため、StaticModel描画処理に失敗しました。");
				return false;
			}

			if (!a_modelMaterialRuntimeData.m_normalTexture)
			{
				assert(false && "NormalTextureが無効なため、StaticModel描画処理に失敗しました。");
				return false;
			}
			const auto& l_baseColorTextureRecord = a_modelMaterialRuntimeData.m_baseColorTexture->GetREFTextureRecord().lock();

			if (!l_baseColorTextureRecord)
			{
				assert(false && "BaseColorTexture用TextureRecordが無効なため、StaticModel描画処理に失敗しました。");
				return false;
			}

			const auto& l_normalTextureRecord = a_modelMaterialRuntimeData.m_normalTexture->GetREFTextureRecord().lock();

			if (!l_normalTextureRecord)
			{
				assert(false && "NormalTexture用TextureRecordが無効なため、StaticModel描画処理に失敗しました。");
				return false;
			}

			if (!ValidateModelMeshStructuredBufferSRV(a_modelMeshRuntimeData))
			{
				assert(false && "ModelMesh用StructuredBufferのSRVStorageIDが無効なため、StaticModel描画処理に失敗しました。");
				return false;
			}

			Struct::CBModelObject l_cbModelObject = {};

			l_cbModelObject.m_worldMatrix		              = a_staticModelStandardDrawCommand.m_worldMatrix;
			l_cbModelObject.m_baseColorTextureSRVIndex        = l_baseColorTextureRecord->m_srvStorageID;
			l_cbModelObject.m_normalTextureSRVIndex           = l_normalTextureRecord->m_srvStorageID;
			l_cbModelObject.m_vertexBufferSRVIndex            = a_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID;
			l_cbModelObject.m_meshletBufferSRVIndex           = a_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID;
			l_cbModelObject.m_uniqueVertexIndexBufferSRVIndex = a_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID;
			l_cbModelObject.m_primitiveIndexBufferSRVIndex    = a_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID;

			return SetupConstantBuffer<Tag::RootParameterCBModelObjectTag>(a_rootSignature, 
																		   a_directCommandList,
																		   a_modelObjectUploadBuffer,
																		   l_cbModelObject,
																		   a_modelObjectIndex,
																		   a_modelObjectMappedData);
		}

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