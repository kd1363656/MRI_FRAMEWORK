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

void FWK::Graphics::UploadSystem::UpdateCompletedUploads()
{
	std::size_t l_index = k_pendingUploadBufferBeginIndex;
	
	while (l_index < m_pendingUploadBufferList.size())
	{
		// Fenceが未完了なら、まだCopyQueue上でUploadBufferを使用しているため次の要素を確認する
		if (!m_copyCommandQueue.IsFenceValueCompleted(m_pendingUploadBufferList[l_index].m_fenceValue))
		{
			++l_index;
			continue;
		}

		// リストのサイズが変わる可能性があるため、末尾のインデックスを都度取得する
		if (const auto& l_lastIndex = m_pendingUploadBufferList.size() - k_pendingUploadBufferListSizeToLastIndexSubtraction;
			l_index != l_lastIndex)
		{
			// 完了済み要素を末尾へ移動してから、末尾を削除する
			// vector::erase()のような後続要素の前詰めを避けるための高速削除
			std::swap(m_pendingUploadBufferList[l_index], m_pendingUploadBufferList[l_lastIndex]);
		}

		// 末尾を削除
		m_pendingUploadBufferList.pop_back();

		// 注意
		// 末尾から未確認要素が現在位置へ移動してきた可能性があるため、
		// l_indexは進めずに、同じindexを次のループで確認する
	}
}

nlohmann::json FWK::Graphics::UploadSystem::Serialize() const
{
	return m_uploadSystemJsonConverter.Serialize(*this);
}

bool FWK::Graphics::UploadSystem::IsUploadCompleted(const UINT64& a_fenceValue) const
{
	return m_copyCommandQueue.IsFenceValueCompleted(a_fenceValue);
}

bool FWK::Graphics::UploadSystem::SubmitTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>&              a_textureResource,
													const UploadBuffer&						               a_uploadBuffer, 
													const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList, 
														  UINT64&										   a_submittedFenceValue)
{
	a_submittedFenceValue = Constant::k_unusedFenceValue;

	if (!a_textureResource)
	{
		assert(false && "コピー先TextureResourceが無効のため、テクスチャコピー送信処理に失敗しました。");
		return false;
	}
	
	if (!a_uploadBuffer.GetREFUploadBuffer())
	{
		assert(false && "コピー元UploadBufferが無効のため、テクスチャコピー送信処理に失敗しました。");
		return false;
	}

	if (a_layoutList.empty())
	{
		assert(false && "サブリソースは位置情報が空のため、テクスチャコピー送信処理に失敗しました。");
		return false;
	}

	UpdateCompletedUploads();

	auto* l_copyCommandAllocator = FetchMutablePTRCopyCommandAllocator();

	if (!l_copyCommandAllocator)
	{
		assert(false && "使用可能なコピーコマンドアロケータが取得できず、テクスチャコピー送信処理に失敗しました。");
		return false;
	}

	l_copyCommandAllocator->Reset();

	m_copyCommandList.Reset(*l_copyCommandAllocator);

	RecordTextureCopy(a_textureResource, a_uploadBuffer.GetREFUploadBuffer(), a_layoutList);

	m_copyCommandList.Close();

	m_copyCommandQueue.ExecuteCommandLists(m_copyCommandList);

	m_copyCommandQueue.SignalAndTrackAllocator(*l_copyCommandAllocator);

	a_submittedFenceValue = l_copyCommandAllocator->GetREFSubmittedFenceValue();

	PendingUploadBuffer l_pendingUploadBuffer = {};

	// UploadBufferをPending側にも保持する
	// CopyQueue上のコピー命令が完了するまでUploadBufferを破棄しないため
	l_pendingUploadBuffer.m_uploadBuffer = a_uploadBuffer;
	l_pendingUploadBuffer.m_fenceValue   = a_submittedFenceValue;

	m_pendingUploadBufferList.emplace_back(l_pendingUploadBuffer);

	return true;
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
		const D3D12_TEXTURE_COPY_LOCATION l_destinationCopyLocation = { a_textureResource.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, l_subresourceIndex };

		// D3D12_TEXTURE_COPY_LOCATIONについて
		// pResource        : コピー先になるTextureResource
		// Type             : コピー先はTextureResourceのサブリソースなのでSUBRESOURCE_INDEXを指定する
		// SubresourceIndex : コピー先TextureResourceのサブリソース番号
		const D3D12_TEXTURE_COPY_LOCATION l_sourceCopyLocation = { a_uploadBuffer.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, a_layoutList[l_subresourceIndex]};

		// CopyTextureRegion(コピー先の位置情報、
		//					 コピー先X座標、
		//					 コピー先Y座標、
		//					 コピー先Z座標、
		//					 コピー元の位置情報、
		//					 コピー元のコピー範囲(nullptrならサブリソース全体));

		m_copyCommandList.CopyTextureRegion(l_destinationCopyLocation,
											l_sourceCopyLocation,
										    nullptr,
											0U,
											0U,
											0U);
	}
}

FWK::Graphics::CopyCommandAllocator* FWK::Graphics::UploadSystem::FetchMutablePTRCopyCommandAllocator()
{
	if (m_copyCommandAllocatorList.empty())
	{
		assert(false && "コピーコマンドアロケータリストが空のため、コピーコマンドアロケータ取得処理に失敗しました。");
		return nullptr;
	}

	for (std::size_t l_count = 0ULL; l_count < m_copyCommandAllocatorList.size(); ++l_count)
	{
		const auto& l_index = (m_currentCopyCommandAllocatorIndex + l_count) % m_copyCommandAllocatorList.size();

		auto& l_copyCommandAllocator = m_copyCommandAllocatorList[l_index];

		if (!m_copyCommandQueue.IsFenceValueCompleted(l_copyCommandAllocator.GetREFSubmittedFenceValue())) { continue; }

		m_currentCopyCommandAllocatorIndex = (l_index + k_nextCopyCommandAllocatorIndexStep) % m_copyCommandAllocatorList.size();

		return &l_copyCommandAllocator;
	}

	auto& l_copyCommandAllocator = m_copyCommandAllocatorList[m_currentCopyCommandAllocatorIndex];

	// 全てのコピーコマンドアロケータがGPU使用中の場合だけ待機する。
	// GPUが使用中のCommandAllocatorをResetすると危険なため、ここでは安全のため待つ
	m_copyCommandQueue.EnsureAllocatorAvailable(l_copyCommandAllocator);

	m_currentCopyCommandAllocatorIndex = (m_currentCopyCommandAllocatorIndex + k_nextCopyCommandAllocatorIndexStep) % m_copyCommandAllocatorList.size();

	return &l_copyCommandAllocator;
}