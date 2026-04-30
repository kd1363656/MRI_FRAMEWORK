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
	for (auto& l_copyCommandAllocator : m_copyCommandAllocatorList)
	{
		if (!l_copyCommandAllocator.Create(a_device))
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

bool FWK::Graphics::UploadSystem::SubmitTextureCopyBatchAndWait(const std::vector<Struct::TextureUploadRecord>& a_textureUploadRecordList)
{
	if (a_textureUploadRecordList.empty())
	{
		assert(false && "テクスチャアップロード情報リストが空のため、バッチテクスチャコピー送信処理に失敗しました。");
		return false;
	}

	// 無効なテクスチャアップロード情報が含まれていないかを確認する。
	for (const auto& l_textureUploadRecord : a_textureUploadRecordList)
	{
		if (!ValidateTextureUploadRecord(l_textureUploadRecord))
		{
			assert(false && "無効なテクスチャアップロード情報が含まれているため、バッチテクスチャコピー送信処理に失敗しました。");
			return false;
		}
	}

	auto* l_copyCommandAllocator = FetchMutablePTRCopyCommandAllocator();

	if (!l_copyCommandAllocator)
	{
		assert(false && "使用可能なコピーコマンドアロケータが取得できず、バッチテクスチャコピー送信処理に失敗しました。");
		return false;
	}

	l_copyCommandAllocator->Reset();
	m_copyCommandList.Reset      (*l_copyCommandAllocator);

	for (const auto& l_textureUploadRecord : a_textureUploadRecordList)
	{
		RecordTextureCopy(l_textureUploadRecord.m_textureResource, l_textureUploadRecord.m_uploadBuffer.GetREFUploadBuffer(), l_textureUploadRecord.m_layoutList);
	}

	m_copyCommandList.Close();
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

void FWK::Graphics::UploadSystem::RecordTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer, const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList) const
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

	for (UINT l_subresourceIndex = 0U; l_subresourceIndex < static_cast<UINT>(a_layoutList.size()); ++l_subresourceIndex)
	{
		// D3D12_TEXTURE_COPY_LOCATIONについて
		// pResource        : コピー先になるTextureResource
		// Type             : コピー先はTextureResourceのサブリソースなのでSUBRESOURCE_INDEXを指定する
		// SubresourceIndex : コピー先TextureResourceのサブリソース番号
		const D3D12_TEXTURE_COPY_LOCATION l_destinationCopyLocation = { a_textureResource.Get(), k_destinationTextureCopyType, l_subresourceIndex };

		// D3D12_TEXTURE_COPY_LOCATIONについて
		// pResource        : コピー元になるUploadBuffer
		// Type             : コピー元はUploadBuffer内の配置済みフットプリントなのでPLACED_FOOTPRINTを指定する
		// PlacedFootprint  : UploadBuffer内にあるコピー元サブリソースの配置情報
		const D3D12_TEXTURE_COPY_LOCATION l_sourceCopyLocation = { a_uploadBuffer.Get(), k_sourceTextureCopyType, a_layoutList[l_subresourceIndex]};

		// CopyTextureRegion(コピー先の位置情報、
		//					 コピー先X座標、
		//					 コピー先Y座標、
		//					 コピー先Z座標、
		//					 コピー元の位置情報、
		//					 コピー元のコピー範囲(nullptrならサブリソース全体));

		m_copyCommandList.CopyTextureRegion(l_destinationCopyLocation,
											l_sourceCopyLocation,
										    nullptr,
											k_textureCopyDestinationX,
											k_textureCopyDestinationY,
											k_textureCopyDestinationZ);
	}
}

bool FWK::Graphics::UploadSystem::ValidateTextureUploadRecord(const Struct::TextureUploadRecord& a_textureUploadRecord) const
{
	if (!a_textureUploadRecord.m_textureResource)
	{
		assert(false && "コピー先TextureResourceが無効です。");
		return false;
	}

	if (!a_textureUploadRecord.m_uploadBuffer.GetREFUploadBuffer())
	{
		assert(false && "コピー元UploadBufferが無効です。");
		return false;
	}

	if (a_textureUploadRecord.m_layoutList.empty())
	{
		assert(false && "サブリソースは位置情報が空です。");
		return false;
	}

	return true;
}

FWK::Graphics::CopyCommandAllocator* FWK::Graphics::UploadSystem::FetchMutablePTRCopyCommandAllocator()
{
	if (m_copyCommandAllocatorList.empty())
	{
		assert(false && "コピーコマンドアロケータリストが空のため、コピーコマンドアロケータ取得処理に失敗しました。");
		return nullptr;
	}

	if (m_currentCopyCommandAllocatorIndex >= m_copyCommandAllocatorList.size())
	{
		assert(false && "コピーコマンドアロケータリストの容量を超えたインデックスのため、コピーコマンドアロケータ取得処理に失敗しました。");
		return nullptr;
	}

	auto& l_copyCommandAllocator = m_copyCommandAllocatorList[m_currentCopyCommandAllocatorIndex];

	// もしWaitが必要なコマンドアロケータならWaitする
	m_copyCommandQueue.EnsureAllocatorAvailable(l_copyCommandAllocator);

	// 次のコピーコマンドアロケータを使用するようにインデックスを更新
	m_currentCopyCommandAllocatorIndex = (m_currentCopyCommandAllocatorIndex + k_copyCommandAllocatorIndexIncrement) % m_copyCommandAllocatorList.size();

	return &l_copyCommandAllocator;
}