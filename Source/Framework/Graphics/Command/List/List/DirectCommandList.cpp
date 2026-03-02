#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList(const Device& a_device) : 
	CommandListBase(a_device, D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;