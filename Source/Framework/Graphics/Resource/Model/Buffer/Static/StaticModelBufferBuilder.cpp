#include "StaticModelBufferBuilder.h"

bool FWK::Graphics::StaticModelBufferBuilder::CreateStaticModelRecordBufferUpload(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord, 
																			      const Device&									  a_device, 
																			      const GPUMemoryAllocator&						  a_gpuMemoryAllocator, 
																					    std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList) const
{
	const auto& l_staticModelRecord = a_staticModelRecord.lock();

	if (!l_staticModelRecord)
	{
		assert(false && "StaticModelRecordが無効のため、StaticModel用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	auto& l_modelData = l_staticModelRecord->m_modelData;

	if (l_modelData.m_modelMeshList.empty())
	{
		assert(false && "ModelMeshListが空のため、StaticModel用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	for (auto& l_modelMesh : l_modelData.m_modelMeshList)
	{
		if (!CreateModelMeshBufferUpload(a_device,
										 a_gpuMemoryAllocator,
										 l_modelMesh,
										 a_bufferUploadCommandList))
		{
			assert(false && "ModelMesh用BufferUploadCommandの追加に失敗しました。");
			return false;
		}
	}

	return true;
}

bool FWK::Graphics::StaticModelBufferBuilder::CreateModelMeshBufferUpload(const Device&				                      a_device, 
																	      const GPUMemoryAllocator&                       a_gpuMemoryAllocator,
																				Struct::ModelMesh&                        a_modelMesh, 
																				std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList) const
{
	if (a_modelMesh.m_modelVertexList.empty())
	{
		assert(false && "ModelVertexListが空のため、ModelMesh用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_indexList.empty())
	{
		assert(false && "IndexListが空のため、ModelMesh用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_modelMeshletData.m_meshletList.empty())
	{
		assert(false && "MeshletListが空のため、ModelMesh用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_modelMeshletData.m_uniqueVertexIndexList.empty())
	{
		assert(false && "ModelMesh用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_modelMeshletData.m_meshletBoundsList.empty())
	{
		assert(false && "MeshletBoundsListが空のため、ModelMesh用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	if (a_modelMesh.m_modelMeshletData.m_primitiveIndexList.empty())
	{
		assert(false && "PrimitiveIndexListが空のため、ModelMesh用BufferUploadCommandの追加に失敗しました。");
		return false;
	}

	auto& l_modelMeshRuntimeData = a_modelMesh.m_modelMeshRuntimeData;
	auto& l_modelMeshletData     = a_modelMesh.m_modelMeshletData;

	const auto l_vertexBufferSize = sizeof(Struct::ModelVertex) * a_modelMesh.m_modelVertexList.size();

	// 頂点バッファーのアップロードリソース作成
	if (!CreateBufferUploadCommand(a_device,
								   a_gpuMemoryAllocator,
								   a_modelMesh.m_modelVertexList,
								   a_bufferUploadCommandList,
								   l_modelMeshRuntimeData.m_vertexBufferResource,
								   l_modelMeshRuntimeData.m_vertexBufferAllocation))
	{
		assert(false && "VertexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// インデックスバッファーのアップロードリソース作成
	if(!CreateBufferUploadCommand(a_device,
							      a_gpuMemoryAllocator,
							      a_modelMesh.m_indexList,
							      a_bufferUploadCommandList,
							      l_modelMeshRuntimeData.m_indexBufferResource,
							      l_modelMeshRuntimeData.m_indexBufferAllocation))
	{
		assert(false && "IndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// メッシュレットのアップロードリソース作成
	if (!CreateBufferUploadCommand(a_device,
							       a_gpuMemoryAllocator,
							       l_modelMeshletData.m_meshletList,
							       a_bufferUploadCommandList,
							       l_modelMeshRuntimeData.m_meshletBufferResource,
							       l_modelMeshRuntimeData.m_meshletBufferAllocation))
	{
		assert(false && "MeshletBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}


	// ユニーク頂点インデックスバッファーのアップロードリソースの作成
	if (!CreateBufferUploadCommand(a_device,
							       a_gpuMemoryAllocator,
							       l_modelMeshletData.m_uniqueVertexIndexList,
							       a_bufferUploadCommandList,
							       l_modelMeshRuntimeData.m_uniqueVertexIndexBufferResource,
							       l_modelMeshRuntimeData.m_uniqueVertexIndexBufferAllocation))
	{
		assert(false && "UniqueVertexIndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// プリミティブインデックスバッファーのアップロードリソースの作成
	if (!CreateBufferUploadCommand(a_device,
							       a_gpuMemoryAllocator,
							       l_modelMeshletData.m_primitiveIndexList,
							       a_bufferUploadCommandList,
							       l_modelMeshRuntimeData.m_primitiveIndexBufferResource,
							       l_modelMeshRuntimeData.m_primitiveIndexBufferAllocation))
	{
		assert(false && "PrimitiveIndexBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// メッシュレットバウンドバッファーのアップロードリソースの作成
	if (!CreateBufferUploadCommand(a_device,
							       a_gpuMemoryAllocator,
							       l_modelMeshletData.m_meshletBoundsList,
							       a_bufferUploadCommandList,
							       l_modelMeshRuntimeData.m_meshletBoundsBufferResource,
							       l_modelMeshRuntimeData.m_meshletBoundsBufferAllocation))
	{
		assert(false && "MeshletBoundsBuffer用BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::StaticModelBufferBuilder::CreateBufferUploadCommand(const Device&			                       a_device,
																	   const GPUMemoryAllocator&                       a_gpuMemoryAllocator,
																	   const UINT64&			                       a_bufferSize,
																	   const void*				                       a_sourceData,
																			 std::vector<Struct::BufferUploadCommand>& a_bufferUploadCommandList, 
																			 TypeAlias::ComPtr<ID3D12Resource2>&	   a_destinationBufferResource,
																			 TypeAlias::ComPtr<D3D12MA::Allocation>&   a_destinationBufferAllocation) const
{
	if (a_bufferSize == Constant::k_invalidBufferSize)
	{
		assert(false && "BufferResourceの作成サイズが0のため、BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	if (!a_sourceData)
	{
		assert(false && "コピー元データが無効のため、BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// DEFAULTヒープ上に、最終的にMeshShaderから読むBufferResourceを作成する
	// 初期状態をCOPY_DESTにする理由はUploadBufferからCopyBufferRegionで書き込むコピー先として使用するため
	if (!a_gpuMemoryAllocator.CreateBufferResource(a_bufferSize,
												   D3D12_RESOURCE_STATE_COPY_DEST,
												   a_destinationBufferResource,
												   a_destinationBufferAllocation))
	{
		assert(false && "DEFAULTヒープ上のBufferResource作成に失敗しました。");
		return false;
	}

	Struct::BufferUploadCommand l_bufferUploadCommand = {};

	auto& l_uploadBuffer = l_bufferUploadCommand.m_bufferUploadRecord.m_uploadBuffer;

	// アップロードバッファーの作成
	if (!l_uploadBuffer.Create(a_device, a_bufferSize))
	{
		assert(false && "BufferResourceへコピーするためUploadBuffer作成に失敗しました。");
		return false;
	}

	auto* const l_mappedData = l_uploadBuffer.Map();

	if (!l_mappedData)
	{
		assert(false && "UploadBufferのMapに失敗したため、BufferUploadCommandの作成に失敗しました。");
		return false;
	}

	// バッファ内容をコピー
	std::memcpy(l_mappedData, a_sourceData, a_bufferSize);

	// マップを解除
	l_uploadBuffer.UnMap();

	l_bufferUploadCommand.m_bufferUploadRecord.m_bufferSize = a_bufferSize;

	l_bufferUploadCommand.m_destinationBuffer			= a_destinationBufferResource;
	l_bufferUploadCommand.m_destinationBufferAllocation = a_destinationBufferAllocation;

	a_bufferUploadCommandList.emplace_back(std::move(l_bufferUploadCommand));

	return true;
}