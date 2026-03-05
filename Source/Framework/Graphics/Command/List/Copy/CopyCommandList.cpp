#include "CopyCommandList.h"

FWK::Graphics::CopyCommandList::CopyCommandList(const Device& a_device) : 
	CommandListBase(a_device, D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandList::~CopyCommandList() = default;

void FWK::Graphics::CopyCommandList::CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION& l_dest, D3D12_TEXTURE_COPY_LOCATION& l_src) const
{
	const auto& l_commandList = GetCommandList();

	if (!l_commandList)
	{
		assert(false && "コマンドリストが作成されておらずCopyTextureRegionが実行できませんでした。");
		return;
	}

	l_commandList->CopyTextureRegion(&l_dest, 0U, 0U, 0U, &l_src, nullptr);
}