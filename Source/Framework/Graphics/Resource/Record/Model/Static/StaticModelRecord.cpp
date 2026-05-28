#include "StaticModelRecord.h"

bool FWK::Graphics::StaticModelRecord::PushDeferredRelease(DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, const UINT64& a_retiredFenceValue)
{
	if (a_retiredFenceValue == Constant::k_unusedFenceValue)
	{
		assert(false && "FenceValueが無効のため、StaticModelRecordの遅延解放Queue登録に失敗しました。");
		return false;
	}

	for (auto& l_modelMesh : m_modelData.m_modelMeshList)
	{
		auto& l_modelMEshRuntimeData = l_modelMesh.m_modelMeshRuntimeData;

		if (!IsValidStructuredBufferResource(l_modelMEshRuntimeData.m_vertexBuffer))            { return false; }
		if (!IsValidStructuredBufferResource(l_modelMEshRuntimeData.m_meshletBuffer))           { return false; }
		if (!IsValidStructuredBufferResource(l_modelMEshRuntimeData.m_uniqueVertexIndexBuffer)) { return false; }
		if (!IsValidStructuredBufferResource(l_modelMEshRuntimeData.m_primitiveIndexBuffer))    { return false; }
		if (!IsValidStructuredBufferResource(l_modelMEshRuntimeData.m_meshletBoundsBuffer))     { return false; }

		if (!PushStructuredBufferResource(a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_vertexBuffer, a_retiredFenceValue))
		{
			assert(false && "StaticModelRecordのVertexBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_meshletBuffer, a_retiredFenceValue))
		{
			assert(false && "StaticModelRecordのMeshletBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_uniqueVertexIndexBuffer, a_retiredFenceValue))
		{
			assert(false && "StaticModelRecordのUniqueVertexIndexBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_primitiveIndexBuffer, a_retiredFenceValue))
		{
			assert(false && "StaticModelRecordのPrimitiveIndexBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_meshletBoundsBuffer, a_retiredFenceValue))
		{
			assert(false && "StaticModelRecordのMeshletBoundsBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}
	}

	return true;
}

bool FWK::Graphics::StaticModelRecord::IsValidStructuredBufferResource(const Struct::StructuredBufferResource& a_structuredBufferResource) const
{
	if (!a_structuredBufferResource.m_bufferGPUResource.m_resource) { return false; }

	if (a_structuredBufferResource.m_srvStorageID == Constant::k_invalidStorageID) { return false; }

	return true;
}

bool FWK::Graphics::StaticModelRecord::PushStructuredBufferResource(DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, Struct::StructuredBufferResource& a_strcturedBufferResource, const UINT64& a_retiredFenceValue)
{
	if (!IsValidStructuredBufferResource(a_strcturedBufferResource))
	{
		assert(false && "StructuredBufferResourceが無効のため、遅延解放Queueへの登録に失敗しました。");
		return false;
	}

	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(a_strcturedBufferResource.m_bufferGPUResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_srvDescriptorIndexReleaseRecord = {};

	l_srvDescriptorIndexReleaseRecord.m_storageID		  = a_strcturedBufferResource.m_srvStorageID;
	l_srvDescriptorIndexReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	if (!a_deferredResourceReleaseQueue.PushGPUResourceRecord(std::move(l_gpuResourceReleaseRecord)))
	{
		assert(false && "StructuredBufferResourceのGPUResourceを遅延解放Queueへ登録できませんでした。");
		return false;
	}

	if (!a_deferredResourceReleaseQueue.PushSRVDescriptorIndex(std::move(l_srvDescriptorIndexReleaseRecord)))
	{
		assert(false && "StructuredBufferResourceのSRVDescriptorIndexを遅延解放Queueへ登録できませんでした。");
		return false;
	}

	// 二重開放を防ぐため、Queueへ渡したDescriptorIndexIDは無効化する
	a_strcturedBufferResource.m_srvStorageID = Constant::k_invalidStorageID;

	return true;
}