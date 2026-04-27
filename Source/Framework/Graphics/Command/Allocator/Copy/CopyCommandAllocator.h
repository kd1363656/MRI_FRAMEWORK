#pragma once

namespace FWK::Graphics
{
	class CopyCommandAllocator final : public CommandAllocatorBase
	{
	public:

		 CopyCommandAllocator();
		~CopyCommandAllocator() override;

	private:

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_COPY;
	};
}