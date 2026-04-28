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
	std::size_t l_index = 0ULL;
	
	while (l_index < m_pendingUploadBufferList.size())
	{
		// Fenceが未完了なら、まだCopyQueue上でUploadBuffferを使用しているため次の要素を確認する
		if (!m_copyCommandQueue.IsFenceValueCompleted(m_pendingUploadBufferList[l_index].m_fenceValue))
		{
			++l_index;
			continue;
		}

		// リストのサイズが変わる可能性があるため、末尾のインデックスを都度取得する
		const auto& l_lastIndex = m_pendingUploadBufferList.size() - 1ULL;

		if (l_index != l_lastIndex)
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
	return false;
}

bool FWK::Graphics::UploadSystem::SubmitTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>&              a_textureResource,
													const UploadBuffer&						               a_uploadBuffer, 
													const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList, 
														  UINT64&										   a_submittedFenceValue)
{
	return false;
}

void FWK::Graphics::UploadSystem::RecordTextureCopy(const TypeAlias::ComPtr<ID3D12Resource2>& a_textureResource, const TypeAlias::ComPtr<ID3D12Resource2>& a_uploadBuffer, const std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& a_layoutList)
{

}

FWK::Graphics::CopyCommandAllocator* FWK::Graphics::UploadSystem::FetchMutablePTRCopyCommandAllocator()
{
	return nullptr;
}