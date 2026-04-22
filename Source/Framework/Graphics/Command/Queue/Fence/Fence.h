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

		void SetFenceValue(const auto& a_set) { m_fenceValue = a_set; }

		const auto& GetFenceValue() const { return m_fenceValue; }

		const auto& GetREFFence() const { return m_fence; }

	private:

		HANDLE m_fenceEvent;

		UINT64 m_fenceValue;

		TypeAlias::ComPtr<ID3D12Fence1> m_fence;
	};
}