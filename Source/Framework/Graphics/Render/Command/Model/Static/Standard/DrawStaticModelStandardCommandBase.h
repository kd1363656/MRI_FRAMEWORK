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
			
			auto l_lightSystemConstantBuffer = l_currentFrameResource->FindPTRConstantBuffer<LightConstantBuffer>       ().lock();

			if (!l_cameraConstantBuffer)
			{
				assert(false && "Camera用定数バッファが取得できないため、StaticModel描画処理に失敗しました。");
				return;
			}

			if (!l_lightSystemConstantBuffer)
			{
				assert(false && "Light用定数バッファが取得できないため、StaticModelLit描画処理に失敗しました。");
				return;
			}

			const auto& l_cameraUploadBuffer	  = l_cameraConstantBuffer->GetREFUploadConstantBuffer	   ();
			const auto& l_lightSystemUploadBuffer = l_lightSystemConstantBuffer->GetREFUploadConstantBuffer();

			auto* const l_cameraMappedData	    = l_cameraUploadBuffer.Map	   ();
			auto* const l_lightSystemMappedData = l_lightSystemUploadBuffer.Map();
			
			if (!l_cameraMappedData)
			{
				assert(false && "Camera用定数バッファのMapに失敗したため、StaticModel描画処理に失敗しました。");
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
		void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) override
		{
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

			// セットされていなければreturn
			if (!GetPTRPassConstant()) { return; }
			const auto& l_cbCamera = GetPTRPassConstant()->m_camera.lock();

			const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

			// もし共通定数バッファの設定に失敗したらマップを解除
			if (!SetupCommonPassConstantBuffer<CameraConstantBuffer, Tag::RootParameterCBCameraTag>(*l_rootSignature,
																								    l_directCommandList,
																								    *l_currentFrameResource,
																								    l_cbCamera->CreateCBCamera()))
			{
				assert(false && "共通パスの定数バッファが設定できず、描画処理に失敗しました。");
				return;
			}
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

		bool SetupCBModelObject(const RootSignature&	 a_rootSignature, ,
								const DirectCommandList& a_directCommandList,
								const FrameResource&	 a_frameResource) const
		{
			// モデルオブジェクトマップ用定数バッファの取得
			auto l_modelObjectConstantBuffer = a_frameResource.FindPTRConstantBuffer<ModelObjectConstantBuffer>().lock();

			if (!l_modelObjectConstantBuffer)
			{
				assert(false && "ModelObject用定数バッファが取得できないため、StaticModel描画処理に失敗しました。");
				return false;
			}

			const auto& l_modelObjectUploadBuffer = l_modelObjectConstantBuffer->GetREFUploadConstantBuffer();

			auto* const l_modelObjectMappedData = l_modelObjectUploadBuffer.Map();

			
			const auto& l_staticModelDrawCommandList = GetREFDrawCommandList();

			std::size_t l_modelObjectIndex = 0ULL;
			
			for (const auto& l_staticModelDrawCommand : l_staticModelDrawCommandList)
			{
				const auto& l_staticModelRecord = l_staticModelDrawCommand.m_staticModelRecord.lock();

				if (!l_staticModelRecord) { continue; }

				const auto& l_modelMeshList = l_staticModelRecord->m_modelData.m_modelMeshList;

				for (const auto& l_modelMesh : l_modelMeshList)
				{
					const auto& l_modelMeshletData         = l_modelMesh.m_modelMeshletData;
					const auto& l_modelMeshRuntimeData     = l_modelMesh.m_modelMeshRuntimeData;
					const auto& l_modelMaterialRuntimeData = l_modelMesh.m_modelMaterial.m_modelMaterialAssetData;

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

					l_cbModelObject.m_worldMatrix		              = l_staticModelStandardDrawCommand.m_worldMatrix;
					l_cbModelObject.m_baseColorTextureSRVIndex        = l_baseColorTextureRecord->m_srvStorageID;
					l_cbModelObject.m_normalTextureSRVIndex           = l_normalTextureRecord->m_srvStorageID;
					l_cbModelObject.m_vertexBufferSRVIndex            = l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID;
					l_cbModelObject.m_meshletBufferSRVIndex           = l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID;
					l_cbModelObject.m_uniqueVertexIndexBufferSRVIndex = l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID;
					l_cbModelObject.m_primitiveIndexBufferSRVIndex    = l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID;

					return SetupConstantBuffer<Tag::RootParameterCBModelObjectTag>(a_rootSignature, 
																				   a_directCommandList,
																				   a_modelObjectUploadBuffer,
																				   l_cbModelObject,
																				   a_modelObjectIndex,
																				   a_modelObjectMappedData);

					a_directCommandList.DispatchMesh(static_cast<UINT>(l_modelMeshletData.m_meshletList.size()), GetVALDefaultDispatchMeshThreadGroupCountY(), GetVALDefaultDispatchMeshThreadGroupCountZ());

					++l_modelObjectIndex;
				}
			}
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