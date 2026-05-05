#pragma once

namespace FWK::Graphics
{
	class Fence final
	{
	public:

		 Fence();
		~Fence();

		bool Create(const Device& a_device);

		void WaitForFenceValueIfNeeded(const UINT64& a_fenceValue);

		bool IsFenceValueCompleted(const UINT64& a_fenceValue) const;

		void SetLastSignaledFenceValue(const auto& a_set) { m_lastSignaledFenceValue = a_set; }

		UINT64 FetchVALCompletedFenceValue() const;

		const auto& GetREFLastSignaledFenceValue() const { return m_lastSignaledFenceValue; }

		const auto& GetREFFence() const { return m_fence; }

	private:

		HANDLE m_event;

		TypeAlias::ComPtr<ID3D12Fence1> m_fence;

		UINT64 m_lastSignaledFenceValue;
	};
}