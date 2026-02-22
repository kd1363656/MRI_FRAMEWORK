#include "CommandContextBase.h"

FWK::Render::CommandContextBase::CommandContextBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType) : 
	k_createCommandListType(a_createCommandListType)
{}
FWK::Render::CommandContextBase::~CommandContextBase() = default;