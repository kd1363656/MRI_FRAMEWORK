#pragma once

namespace FWK::Graphics
{
	class DirectCommandAllocator final : public CommandAllocatorBase
	{
	public:

		 DirectCommandAllocator();
		~DirectCommandAllocator() override;

	private:

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	};
}