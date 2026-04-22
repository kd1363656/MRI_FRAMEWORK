#pragma once

namespace FWK::Graphics
{
	class CommandQueueBase
	{
	public:
		
		explicit  CommandQueueBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual  ~CommandQueueBase();

		bool Create(const Device& a_device);

	private:

		bool CreateCommandQueue(const Device& a_device);

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		TypeAlias::ComPtr<ID3D12CommandQueue> m_commandQueue;
	};
}