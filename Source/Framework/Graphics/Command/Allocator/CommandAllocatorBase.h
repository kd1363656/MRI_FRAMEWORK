#pragma once

namespace FWK::Graphics
{
	class CommandAllocatorBase
	{
	public:

		explicit CommandAllocatorBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandAllocatorBase();

		bool Create(const Device& a_device);

		void Reset();

	private:

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		UINT64 m_submittedFenceValue;

		TypeAlias::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	};
}