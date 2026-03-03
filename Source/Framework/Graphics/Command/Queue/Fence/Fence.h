#pragma once

namespace FWK::Graphics
{
	class CommandQueueBase;
}

namespace FWK::Graphics
{
	class Fence final
	{
	public:

		explicit Fence(const Device& a_device, const CommandQueueBase& a_commandQueueBase);
		~Fence        ();

		void Init  ();
		bool Create();

		void WaitForGPUIdle();

		UINT64 Signal();

		void WaitForFenceValue(const UINT64 a_fenceValue);

		const auto& GetFence() const { return m_fence; }

		const auto& GetFenceEvent() const { return m_fenceEvent; }

		UINT64 GetFenceValue         () const { return m_fenceValue; }
		UINT64 GetCompletedFenceValue() const;

		void SetFenceValue(const UINT64 a_set) { m_fenceValue = a_set; }

	private:

		const Device&           k_device;
		const CommandQueueBase& k_commandQueueBase;

		ComPtr<ID3D12Fence1> m_fence;

		HANDLE m_fenceEvent;

		UINT64 m_fenceValue;
	};
}