#include "StaticModelReleaser.h"

FWK::Graphics::StaticModelRecordReleaser::StaticModelRecordReleaser(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool) :
	m_srvDescriptorPool(a_srvDescriptorPool)
{}
FWK::Graphics::StaticModelRecordReleaser::~StaticModelRecordReleaser() = default;

bool FWK::Graphics::StaticModelRecordReleaser::ReleaseRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord) const
{
	const auto& l_staticModelRecord = a_staticModelRecord.lock();

	if (!l_staticModelRecord)
	{
		assert(false && "StaticModelRecordが無効のため、StaticModelRecordの解放に失敗しました。");
		return false;
	}

	for (auto& l_modelMesh : l_staticModelRecord->m_modelData.m_modelMeshList)
	{
		auto& l_modelMeshRuntimeData = l_modelMesh.m_modelMeshRuntimeData;

		// バッファーの解放
		if (l_modelMeshRuntimeData.m_vertexBufferResource)
		{
			l_modelMeshRuntimeData.m_vertexBufferResource.Reset();
		}

		if (l_modelMeshRuntimeData.m_indexBufferResource)
		{
			l_modelMeshRuntimeData.m_indexBufferResource.Reset();
		}

		if (l_modelMeshRuntimeData.m_meshletBoundsBufferResource)
		{
			l_modelMeshRuntimeData.m_meshletBoundsBufferResource.Reset();
		}

		if (l_modelMeshRuntimeData.m_uniqueVertexIndexBufferResource)
		{
			l_modelMeshRuntimeData.m_uniqueVertexIndexBufferResource.Reset();
		}

		if (l_modelMeshRuntimeData.m_primitiveIndexBufferResource)
		{
			l_modelMeshRuntimeData.m_primitiveIndexBufferResource.Reset();
		}

		if (l_modelMeshRuntimeData.m_meshletBoundsBufferResource)
		{
			l_modelMeshRuntimeData.m_meshletBoundsBufferResource.Reset();
		}

		// アロケーションの解放
		if (l_modelMeshRuntimeData.m_vertexBufferAllocation)
		{
			l_modelMeshRuntimeData.m_vertexBufferAllocation.Reset();
		}

		if (l_modelMeshRuntimeData.m_indexBufferAllocation)
		{
			l_modelMeshRuntimeData.m_indexBufferAllocation.Reset();
		}

		if (l_modelMeshRuntimeData.m_meshletBoundsBufferAllocation)
		{
			l_modelMeshRuntimeData.m_meshletBoundsBufferAllocation.Reset();
		}

		if (l_modelMeshRuntimeData.m_uniqueVertexIndexBufferAllocation)
		{
			l_modelMeshRuntimeData.m_uniqueVertexIndexBufferAllocation.Reset();
		}

		if (l_modelMeshRuntimeData.m_primitiveIndexBufferAllocation)
		{
			l_modelMeshRuntimeData.m_primitiveIndexBufferAllocation.Reset();
		}

		if (l_modelMeshRuntimeData.m_meshletBoundsBufferAllocation)
		{
			l_modelMeshRuntimeData.m_meshletBoundsBufferAllocation.Reset();
		}

		// SRVの解放
		if (l_modelMeshRuntimeData.m_vertexBufferSRVStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_vertexBufferSRVStorageID);
			l_modelMeshRuntimeData.m_vertexBufferSRVStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_meshletBufferSRVStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_meshletBufferSRVStorageID);
			l_modelMeshRuntimeData.m_meshletBufferSRVStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_uniqueVertexIndexBufferSRVStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_uniqueVertexIndexBufferSRVStorageID);
			l_modelMeshRuntimeData.m_uniqueVertexIndexBufferSRVStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_primitiveIndexBufferSRVStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_primitiveIndexBufferSRVStorageID);
			l_modelMeshRuntimeData.m_primitiveIndexBufferSRVStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_meshletBoundsBufferSRVStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_meshletBoundsBufferSRVStorageID);
			l_modelMeshRuntimeData.m_meshletBoundsBufferSRVStorageID = Constant::k_invalidStorageID;
		}
	}

	return true;
}