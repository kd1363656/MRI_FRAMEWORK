#pragma once

namespace FWK::Graphics
{
	class Fence final
	{
	public:

		Fence()  = default;
		~Fence() = default;

		void Init();
		bool Create(const Device& a_device);

		void WaitForFenceValue(const UINT64& a_fenceValue);
		void WaitForGPUIdle(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& a_commandQueue);

		void SetFenceValue(const UINT64& a_set) { m_fenceValue = a_set; }

		const auto& GetFence() const { return m_fence; }

		const auto& GetFenceValue() const { return m_fenceValue; }

	private:

		Microsoft::WRL::ComPtr<ID3D12Fence1> m_fence = nullptr;

		HANDLE m_fenceEvent = nullptr;

		UINT64 m_fenceValue = CommonConstant::k_initialFenceValue;
	};
}