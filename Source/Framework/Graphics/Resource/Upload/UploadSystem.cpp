#include "UploadSystem.h"

void FWK::Graphics::UploadSystem::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	m_uploadSystemJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::UploadSystem::Create(const Device& a_device)
{
	if (!m_copyCommandQueue.Create(a_device))
	{
		assert(false && "コピーコマンドキュー作成処理に失敗しました。");
		return false;
	}

	// Deserializeでコピーコマンドアロケータの数を設定している、もしコマンドアロケータ数が0なら
	// リストが空の場合は、UploadSystemJsonConverterのDeserialize処理を確認すること
	for (const auto& l_copyCommandAllocator : m_copyCommandAllocatorList)
	{
		if (!l_copyCommandAllocator) 
		{
			assert(false && "コマンドアロケーターが無効のため、作成処理に失敗しました。");
			return false;
		}

		if (!l_copyCommandAllocator->Create(a_device))
		{
			assert(false && "コピーコマンドアロケータ作成処理に失敗しました。");
			return false;
		}
	}

	if (!m_copyCommandList.Create(a_device))
	{
		assert(false && "コピーコマンドリスト作成処理に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::UploadSystem::SubmitTextureCopyBatchAndWait(const TypeAlias::PendingTextureBatchUploadRecordMap& a_pendingTextureBatchUploadRecordMap)
{
	if (a_pendingTextureBatchUploadRecordMap.empty())
	{
		assert(false && "テクスチャのバッチアップロード用の情報リストが空のため、バッチテクスチャコピー送信処理に失敗しました。");
		return false;
	}

	const auto& l_copyCommandAllocator = FetchMutablePTRCopyCommandAllocator().lock();

	if (!l_copyCommandAllocator)
	{
		assert(false && "使用可能なコピーコマンドアロケータが取得できず、バッチテクスチャコピー送信処理に失敗しました。");
		return false;
	}

	// 命令を格納できるようにするためリセット
	l_copyCommandAllocator->Reset();
	m_copyCommandList.Reset      (*l_copyCommandAllocator);

	// UploadBuffer内に配置した各サブリソースの画像データを
	// D3D12_PLACED_SUBRESOURCE_FOOTPRINTの配置情報に従って、DEFAULTヒープ上のテクスチャリソースへコピーする
	for (const auto& [l_filePath, l_pendingTextureBatchUploadRecord] : a_pendingTextureBatchUploadRecordMap)
	{
		const auto& l_textureRecord = l_pendingTextureBatchUploadRecord.m_textureRecord;

		if (!l_textureRecord)
		{
			assert(false && "TextureRecordが無効になっており、テクスチャコピー処理ができませんでした。");
			return false;
		}

		const auto& l_textureUploadRecord = l_pendingTextureBatchUploadRecord.m_textureUploadRecord;

		// 
		RecordTextureCopy(l_textureUploadRecord.m_layoutList, l_textureRecord->m_gpuResource.m_resource, l_textureUploadRecord.m_uploadBuffer.GetREFUploadBuffer());
	}

	m_copyCommandList.Close				  ();
	m_copyCommandQueue.ExecuteCommandLists(m_copyCommandList);

	m_copyCommandQueue.SignalAndTrackAllocator(*l_copyCommandAllocator);

	// Batch + Wait方式なので、ここでGPUコピー完了まで待つ
	// この関数を抜けた後はUploadBufferを保持し続ける必要がない
	m_copyCommandQueue.EnsureAllocatorAvailable(*l_copyCommandAllocator);

	return true;
}
bool FWK::Graphics::UploadSystem::SubmitTextureCopyBatchAndWait(const std::vector<Struct::TextureBatchUploadRecord>& a_textureBatchUploadRecordList)
{
	if (a_textureBatchUploadRecordList.empty())
	{
		assert(false && "テクスチャのバッチアップロード用の情報リストが空のため、バッチテクスチャコピー送信処理に失敗しました。");
		return false;
	}

	const auto& l_copyCommandAllocator = FetchMutablePTRCopyCommandAllocator().lock();

	if (!l_copyCommandAllocator)
	{
		assert(false && "使用可能なコピーコマンドアロケータが取得できず、バッチテクスチャコピー送信処理に失敗しました。");
		return false;
	}

	l_copyCommandAllocator->Reset();
	m_copyCommandList.Reset      (*l_copyCommandAllocator);

	for (const auto& l_textureBatchUploadRecord : a_textureBatchUploadRecordList)
	{
		const auto& l_textureRecord = l_textureBatchUploadRecord.m_textureRecord;

		if (!l_textureRecord)
		{
			assert(false && "TextureRecordが無効になっており、テクスチャコピー処理ができませんでした。");
			return false;
		}

		const auto& l_textureUploadRecord = l_textureBatchUploadRecord.m_textureUploadRecord;

		RecordTextureCopy(l_textureUploadRecord.m_layoutList, l_textureRecord->m_gpuResource.m_resource, l_textureUploadRecord.m_uploadBuffer.GetREFUploadBuffer());
	}

	m_copyCommandList.Close();
	m_copyCommandQueue.ExecuteCommandLists(m_copyCommandList);

	m_copyCommandQueue.SignalAndTrackAllocator (*l_copyCommandAllocator);
	m_copyCommandQueue.EnsureAllocatorAvailable(*l_copyCommandAllocator);

	return true;
}
bool FWK::Graphics::UploadSystem::SubmitStaticModelBufferCopyBatchAndWait(const TypeAlias::PendingStaticModelBatchUploadRecordMap& a_pendingStaticModelBatchUploadRecordMap)
{
	if (a_pendingStaticModelBatchUploadRecordMap.empty())
	{
		assert(false && "StaticModel用PendingBatchUploadRecordMapが空のため、StaticModel用BufferResourceのバッチコピーに失敗しました。");
		return false;
	}

	const auto& l_copyCommandAllocator = FetchMutablePTRCopyCommandAllocator().lock();

	if (!l_copyCommandAllocator)
	{
		assert(false && "使用可能なコピーコマンドアロケータが取得できず、StaticModel用BufferResourceのバッチコピーに失敗しました。");
		return false;
	}

	// 命令を格納できるようにするためにリセット
	l_copyCommandAllocator->Reset();
	m_copyCommandList.Reset      (*l_copyCommandAllocator);

	for (const auto& [l_filePath, l_staticModelBatchUploadRecord] : a_pendingStaticModelBatchUploadRecordMap)
	{
		if (l_staticModelBatchUploadRecord.m_bufferUploadCommandList.empty())
		{
			assert(false && "StaticModel用BufferUploadCommandListが空のため、StaticModel用BufferResourceのバッチコピーに失敗しました。");
			return false;
		}

		for (const auto& l_bufferUploadCommand : l_staticModelBatchUploadRecord.m_bufferUploadCommandList)
		{
			RecordBufferCopy(l_bufferUploadCommand);
		}
	}

	m_copyCommandList.Close				  ();
	m_copyCommandQueue.ExecuteCommandLists(m_copyCommandList);

	m_copyCommandQueue.SignalAndTrackAllocator(*l_copyCommandAllocator);

	// Batch + Wait方式なので、ここでGPUコピー完了まで待つ
	// この関数を抜けた後はUploadBufferを保持し続ける必要がない
	m_copyCommandQueue.EnsureAllocatorAvailable(*l_copyCommandAllocator);

	return true;
}

nlohmann::json FWK::Graphics::UploadSystem::Serialize() const
{
	return m_uploadSystemJsonConverter.Serialize(*this);
}

void FWK::Graphics::UploadSystem::RecordTextureCopy(const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList, const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer) const
{
	if (!a_textureResource)
	{
		assert(false && "コピー先TextureResourceが無効のため、テクスチャコピー記録に失敗しました。");
		return;
	}

	if (!a_uploadBuffer)
	{
		assert(false && "コピー元UploadBufferが無効のため、テクスチャコピー記録に失敗しました。");
		return;
	}

	if (a_layoutList.empty())
	{
		assert(false && "サブリソース配置情報が空のため、テクスチャコピー記録処理に失敗しました。");
		return;
	}

	for (auto l_subresourceIndex = 0U; l_subresourceIndex < static_cast<UINT>(a_layoutList.size()); ++l_subresourceIndex)
	{
		// D3D12_TEXTURE_COPY_LOCATIONについての説明
		// pResource        : コピー先になるTextureResource
		// Type             : コピー先はTextureResourceのサブリソースなのでSUBRESOURCE_INDEXを指定する
		// SubresourceIndex : コピー先TextureResourceのサブリソース番号
		const D3D12_TEXTURE_COPY_LOCATION l_destinationCopyLocation = { a_textureResource.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, l_subresourceIndex };

		// D3D12_TEXTURE_COPY_LOCATIONについての説明
		// pResource        : コピー元になるUploadBuffer
		// Type             : コピー元はUploadBuffer内の配置済みフットプリントなのでPLACED_FOOTPRINTを指定する
		// PlacedFootprint  : UploadBuffer内にあるコピー元サブリソースの配置情報
		const D3D12_TEXTURE_COPY_LOCATION l_sourceCopyLocation = { a_uploadBuffer.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, a_layoutList[l_subresourceIndex]};

		m_copyCommandList.CopyTextureRegion(l_destinationCopyLocation,
											l_sourceCopyLocation,
										    nullptr,
											k_textureCopyDestinationX,
											k_textureCopyDestinationY,
											k_textureCopyDestinationZ);
	}
}
void FWK::Graphics::UploadSystem::RecordBufferCopy(const Struct::BufferUploadCommand& a_bufferUploadCommand) const
{
	if (!a_bufferUploadCommand.m_destinationBufferResource)
	{
		assert(false && "コピー先BufferResourceが無効のため、バッファコピー記録に失敗しました。");
		return;
	}

	const auto& l_uploadBuffer = a_bufferUploadCommand.m_bufferUploadRecord.m_uploadBuffer.GetREFUploadBuffer();

	if (!l_uploadBuffer)
	{
		assert(false && "コピー元UploadBufferが無効のため、バッファコピー記録に失敗しました。");
		return;
	}

	if (a_bufferUploadCommand.m_bufferUploadRecord.m_bufferSize == Constant::k_invalidBufferSize)
	{
		assert(false && "コピーするBufferサイズが0のため、バッファコピー記録に失敗しました。");
		return;
	}

	// コピー先とコピー元のリソースを取得
	auto& l_destinationBufferResource = *a_bufferUploadCommand.m_destinationBufferResource.Get();
	auto& l_sourceBufferResource      = *l_uploadBuffer.Get									  ();

	// UPLOADヒープ上にあるバッファをDEFAULTヒープ上にあるバッファにコピー
	m_copyCommandList.CopyBufferRegion(k_bufferCopyDestinationOffset,
									   k_bufferCopySourceOffset,
									   a_bufferUploadCommand.m_bufferUploadRecord.m_bufferSize,
									   l_destinationBufferResource,
									   l_sourceBufferResource);
}

std::weak_ptr<FWK::Graphics::CopyCommandAllocator> FWK::Graphics::UploadSystem::FetchMutablePTRCopyCommandAllocator()
{
	if (m_copyCommandAllocatorList.empty())
	{
		assert(false && "コピーコマンドアロケータリストが空のため、コピーコマンドアロケータ取得処理に失敗しました。");
		return {};
	}

	if (m_currentCopyCommandAllocatorIndex >= m_copyCommandAllocatorList.size())
	{
		assert(false && "コピーコマンドアロケータリストの容量を超えたインデックスのため、コピーコマンドアロケータ取得処理に失敗しました。");
		return {};
	}

	const auto& l_copyCommandAllocator = m_copyCommandAllocatorList[m_currentCopyCommandAllocatorIndex];

	if (!l_copyCommandAllocator)
	{
		assert(false && "コピーコマンドアロケータが無効のため、コピーコマンドアロケータ取得処理に失敗しました。");
		return {};
	}

	// もしWaitが必要なコマンドアロケータならWaitする
	m_copyCommandQueue.EnsureAllocatorAvailable(*l_copyCommandAllocator);

	// 次のコピーコマンドアロケータを使用するようにインデックスを更新
	m_currentCopyCommandAllocatorIndex = (m_currentCopyCommandAllocatorIndex + k_copyCommandAllocatorIndexIncrement) % m_copyCommandAllocatorList.size();

	return l_copyCommandAllocator;
}