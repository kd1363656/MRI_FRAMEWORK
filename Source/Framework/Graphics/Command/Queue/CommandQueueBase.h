#pragma once

namespace FWK::Graphics
{
	class CommandQueueBase
	{
	public:

		explicit CommandQueueBase (const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual  ~CommandQueueBase();

		void Init  ();
		bool Create();

		const auto& GetCommandQueue() const { return m_commandQueue; }
		const auto& GetFence       () const { return m_fence; }

		auto GetCreateCommandListType() const { return k_createCommandListType; }

	private:

		bool CreateCommandQueue();
		bool CreateFence       ();

		const Device& k_device;

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		ComPtr<ID3D12CommandQueue> m_commandQueue;

		Fence m_fence;
	};
}