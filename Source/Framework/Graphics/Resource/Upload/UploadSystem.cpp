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

nlohmann::json FWK::Graphics::UploadSystem::Serialize() const
{
	return m_uploadSystemJsonConverter.Serialize(*this);
}