#pragma once

namespace FWK::Graphics
{
	class CommandAllocatorBase;
}

namespace FWK::Graphics
{
	class Fence final
	{
	public:

		explicit Fence(const Device& a_device);
		~Fence        ();

		void Init  ();
		bool Create();

		void WaitForFenceValue(const UINT64&					a_fenceValue);
		void WaitForGPUIdle   (const ComPtr<ID3D12CommandQueue> a_commandQueue);

		const auto& GetFence() const { return m_fence; }

		const auto& GetFenceEvent() const { return m_fenceEvent; }

		const UINT64& GetFenceValue() const { return m_fenceValue; }

		UINT64 GetCompletedFenceValue() const;

		void SetFenceValue(const UINT64& a_set) { m_fenceValue = a_set; }

	private:

		const Device& k_device;
		
		ComPtr<ID3D12Fence1> m_fence;

		HANDLE m_fenceEvent;

		UINT64 m_fenceValue;
	};
}