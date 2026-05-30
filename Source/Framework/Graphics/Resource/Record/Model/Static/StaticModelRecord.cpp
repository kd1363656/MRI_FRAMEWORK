#include "StaticModelRecord.h"

bool FWK::Graphics::StaticModelRecord::PushDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	FWK_ASSERT_RETURN_VALUE_IF(a_retiredFenceValue == Constant::k_unusedFenceValue, "FenceValueが無効のため、StaticModelRecordの遅延解放Queue登録に失敗しました。", false);

	for (auto& l_modelMesh : m_modelData.m_modelMeshList)
	{
		auto& l_modelMEshRuntimeData = l_modelMesh.m_modelMeshRuntimeData;

		FWK_ASSERT_RETURN_VALUE_IF(!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_vertexBuffer),			 "StaticModelRecordのVertexBufferを遅延解放Queueへ登録できませんでした。",            false);
		FWK_ASSERT_RETURN_VALUE_IF(!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_meshletBuffer),			 "StaticModelRecordのMeshletBufferを遅延解放Queueへ登録できませんでした。",           false);
		FWK_ASSERT_RETURN_VALUE_IF(!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_uniqueVertexIndexBuffer), "StaticModelRecordのUniqueVertexIndexBufferを遅延解放Queueへ登録できませんでした。", false);
		FWK_ASSERT_RETURN_VALUE_IF(!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_primitiveIndexBuffer),	 "StaticModelRecordのPrimitiveIndexBufferを遅延解放Queueへ登録できませんでした。",    false);
		FWK_ASSERT_RETURN_VALUE_IF(!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_meshletBoundsBuffer),	 "StaticModelRecordのMeshletBoundsBufferを遅延解放Queueへ登録できませんでした。",     false);
	}

	return true;
}

bool FWK::Graphics::StaticModelRecord::IsValidStructuredBufferResource(const Struct::StructuredBufferResource& a_structuredBufferResource) const
{
	if (!a_structuredBufferResource.m_bufferGPUResource.m_resource)				   { return false; }
	if (a_structuredBufferResource.m_srvStorageID == Constant::k_invalidStorageID) { return false; }

	return true;
}

bool FWK::Graphics::StaticModelRecord::PushStructuredBufferResource(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, Struct::StructuredBufferResource& a_structuredBufferResource)
{
	FWK_ASSERT_RETURN_VALUE_IF(!IsValidStructuredBufferResource(a_structuredBufferResource), "StructuredBufferResourceが無効のため、遅延解放Queueへの登録に失敗しました。", false);

	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(a_structuredBufferResource.m_bufferGPUResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_srvDescriptorIndexReleaseRecord = {};

	l_srvDescriptorIndexReleaseRecord.m_storageID		  = a_structuredBufferResource.m_srvStorageID;
	l_srvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushGPUResourceRecord(std::move(l_gpuResourceReleaseRecord)),		 "StructuredBufferResourceのGPUResourceを遅延解放Queueへ登録できませんでした。",			false);
	FWK_ASSERT_RETURN_VALUE_IF(!a_deferredResourceReleaseQueue.PushSRVDescriptorIndex(std::move(l_srvDescriptorIndexReleaseRecord)), "StructuredBufferResourceのSRVDescriptorIndexを遅延解放Queueへ登録できませんでした。", false);

	// 二重解放を防ぐため、Queueへ渡したDescriptorIndexIDは無効化する
	a_structuredBufferResource.m_srvStorageID = Constant::k_invalidStorageID;

	return true;
}