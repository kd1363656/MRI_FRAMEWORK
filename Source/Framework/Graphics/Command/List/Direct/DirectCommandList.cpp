#include "DirectCommandList.h"

FWK::Graphics::DirectCommandList::DirectCommandList() : 
	CommandListBase(D3D12_COMMAND_LIST_TYPE_DIRECT)
{}
FWK::Graphics::DirectCommandList::~DirectCommandList() = default;