#include "UploadSystem.h"

bool FWK::Graphics::UploadSystem::Create(const Device& a_device)
{
	if (!m_copyCommandQueue.Create(a_device))
	{
		assert(false && "コピーコマンドキュー作成処理に失敗しました。");
		return false;
	}

	if (!m_copyCommandAllocator.Create(a_device))
	{
		assert(false && "コピーコマンドアロケータ作成処理に失敗しました。");
		return false;
	}

	if (!m_copyCommandList.Create(a_device))
	{
		assert(false && "コピーコマンドリスト作成処理に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::UploadSystem::RecordTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer, const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList)
{
	if (!a_textureResource)
	{
		assert(false && "コピー先テクスチャリソースが無効のため、テクスチャコピー記録処理に失敗しました。");
		return;
	}

	if (!a_uploadBuffer)
	{
		assert(false && "コピー元UploadBufferが無効のため、テクスチャコピー記録処理に失敗しました。");
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
		// pResource        : コピー先になるテクスチャリソース
		// Type             : コピー先はTextureResourceなのでSUBRESOURCE_INDEXを指定する
		// SubresourceIndex : コピー先のサブリソース番号

		const D3D12_TEXTURE_COPY_LOCATION l_destinationCopyLocation = { a_textureResource.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, l_subresourceIndex };
		const D3D12_TEXTURE_COPY_LOCATION l_sourceCopyLocation      = { a_uploadBuffer.Get   (), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT , a_layoutList[l_subresourceIndex] };

		// CopyTextureRegion(コピー先の位置情報、
		//					 コピー先X座標、
		//					 コピー先Y座標、
		//					 コピー先Z座標、
		//					 コピー元の位置情報、
		//					 コピー範囲(nullptrの場合サブリソース全体をコピー));

		// 各サブリソースの画素データを、UploadBuffer上の対応領域からDefaultHeap上の本番テクスチャへコピーする命令を記録
		m_copyCommandList.CopyTextureRegion(l_destinationCopyLocation,
											l_sourceCopyLocation,
											nullptr,
											0U,
											0U,
											0U);
	}
}

FWK::Graphics::UploadBuffer* FWK::Graphics::UploadSystem::CreateUploadBuffer(const UINT64 a_bufferSize, const Device& a_device)
{
	if (a_bufferSize == 0ULL)
	{
		assert(false && "UploadBufferの作成サイズが0のため、UploadBuffer作成処理に失敗しました。");
		return nullptr;
	}

	PendingUploadBuffer l_pendingUploadBuffer = {};

	if (!l_pendingUploadBuffer.m_uploadBuffer.Create(a_bufferSize, a_device))
	{
		assert(false && "UploadBufferの作成に失敗したため、UploadBuffer作成処理に失敗しました。");
		return nullptr;
	}

	m_pendingUploadBufferList.emplace_back(std::move(l_pendingUploadBuffer));

	return &m_pendingUploadBufferList.back().m_uploadBuffer;
}