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

		// SRVの解放
		if (l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID);
			l_modelMeshRuntimeData.m_vertexBuffer.m_srvStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID);
			l_modelMeshRuntimeData.m_meshletBuffer.m_srvStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID);
			l_modelMeshRuntimeData.m_uniqueVertexIndexBuffer.m_srvStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID);
			l_modelMeshRuntimeData.m_primitiveIndexBuffer.m_srvStorageID = Constant::k_invalidStorageID;
		}

		if (l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_srvStorageID != Constant::k_invalidStorageID)
		{
			m_srvDescriptorPool.Release(l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_srvStorageID);
			l_modelMeshRuntimeData.m_meshletBoundsBuffer.m_srvStorageID = Constant::k_invalidStorageID;
		}
	}

	return true;
}