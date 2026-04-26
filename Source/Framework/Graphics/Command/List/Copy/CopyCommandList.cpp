#include "CopyCommandList.h"

FWK::Graphics::CopyCommandList::CopyCommandList() : 
	CommandListBase(D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandList::~CopyCommandList() = default;