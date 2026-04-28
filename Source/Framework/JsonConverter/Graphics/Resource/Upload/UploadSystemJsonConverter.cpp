#include "UploadSystemJsonConverter.h"

void FWK::JsonConverter::UploadSystemJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::UploadSystem& a_uploadSystem) const
{
	if (a_rootJson.is_null()) { return; }
	
	auto l_copyCommandAllocatorListSize = a_rootJson.value("CopyCommandAllocatorListSize", k_defaultCopyCommandAllocatorListSize);

	// コピーコマンドアロケータが最低一つでもある状態にする
	l_copyCommandAllocatorListSize = std::max(l_copyCommandAllocatorListSize, k_minCopyCommandAllocatorListSize);

	auto& l_copyCommandAllocatorList = a_uploadSystem.GetMutableREFCopyCommandAllocatorList();

	// 保存されているリストの数分コピーコマンドアロケータを作成
	l_copyCommandAllocatorList.resize(l_copyCommandAllocatorListSize);
}

nlohmann::json FWK::JsonConverter::UploadSystemJsonConverter::Serialize(const Graphics::UploadSystem& a_uploadSystem) const
{
	nlohmann::json l_rootJson = {};

	const auto& l_copyCommandAllocatorList = a_uploadSystem.GetREFCopyCommandAllocatorList();

	l_rootJson["CopyCommandAllocatorListSize"] = l_copyCommandAllocatorList.size();

	return l_rootJson;
}