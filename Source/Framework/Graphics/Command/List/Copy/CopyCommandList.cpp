#include "CopyCommandList.h"

FWK::Graphics::CopyCommandList::CopyCommandList(const Device& a_device) : 
	CommandListBase(a_device, D3D12_COMMAND_LIST_TYPE_COPY)
{}
FWK::Graphics::CopyCommandList::~CopyCommandList() = default;