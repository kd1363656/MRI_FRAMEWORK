#include "StaticModelRecord.h"

bool FWK::Graphics::StaticModelRecord::PushDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
{
	if (a_retiredFenceValue == Constant::k_unusedFenceValue)
	{
		assert(false && "FenceValueが無効のため、StaticModelRecordの遅延解放Queue登録に失敗しました。");
		return false;
	}

	for (auto& l_modelMesh : m_modelData.m_modelMeshList)
	{
		auto& l_modelMEshRuntimeData = l_modelMesh.m_modelMeshRuntimeData;

		if (!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_vertexBuffer))
		{
			assert(false && "StaticModelRecordのVertexBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_meshletBuffer))
		{
			assert(false && "StaticModelRecordのMeshletBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_uniqueVertexIndexBuffer))
		{
			assert(false && "StaticModelRecordのUniqueVertexIndexBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_primitiveIndexBuffer))
		{
			assert(false && "StaticModelRecordのPrimitiveIndexBufferを遅延解放Queueへ登録できませんでした。");
			return false;
		}

		if (!PushStructuredBufferResource(a_retiredFenceValue, a_deferredResourceReleaseQueue, l_modelMEshRuntimeData.m_meshletBoundsBuffer))
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

bool FWK::Graphics::StaticModelRecord::PushStructuredBufferResource(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue, Struct::StructuredBufferResource& a_structuredBufferResource)
{
	if (!IsValidStructuredBufferResource(a_structuredBufferResource))
	{
		assert(false && "StructuredBufferResourceが無効のため、遅延解放Queueへの登録に失敗しました。");
		return false;
	}

	Struct::GPUResourceReleaseRecord l_gpuResourceReleaseRecord = {};

	l_gpuResourceReleaseRecord.m_gpuResource	   = std::move(a_structuredBufferResource.m_bufferGPUResource);
	l_gpuResourceReleaseRecord.m_retiredFenceValue = a_retiredFenceValue;

	Struct::DescriptorIndexReleaseRecord l_srvDescriptorIndexReleaseRecord = {};

	l_srvDescriptorIndexReleaseRecord.m_storageID		  = a_structuredBufferResource.m_srvStorageID;
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

	// 二重解放を防ぐため、Queueへ渡したDescriptorIndexIDは無効化する
	a_structuredBufferResource.m_srvStorageID = Constant::k_invalidStorageID;

	return true;
}