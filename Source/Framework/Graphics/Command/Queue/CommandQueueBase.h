#pragma once

namespace FWK::Graphics
{
	class CommandAllocatorBase;
	class CommandListBase;
}

namespace FWK::Graphics
{
	class CommandQueueBase
	{
	public:

		explicit CommandQueueBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandQueueBase();

		void Init();
		bool Create(const Device& a_device);

		void EnsureAllocatorAvailable(const CommandAllocatorBase& a_commandAllocator);

		void ExecuteCommandLists(const CommandListBase& a_commandList) const;

		void SignalAndTrackAllocator(CommandAllocatorBase& a_commandAllocator);

		const auto& GetCommandQueue() const { return m_commandQueue; }

		auto GetCreateCommandListType() const { return k_createCommandListType; }

	private:

		bool CreateCommandQueue(const Device& a_device);
		bool CreateFence(const Device& a_device);

		static constexpr UINT64 k_incrementFenceValue = 1ULL;

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

		Fence m_fence;
	};
}