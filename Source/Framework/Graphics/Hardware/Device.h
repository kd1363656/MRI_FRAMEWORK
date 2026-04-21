#pragma once

namespace FWK::Graphics
{
	class Device final
	{
	public:

		 Device() = default;
		~Device() = default;

		bool Create(const Factory& a_factory, const UINT a_gpuNodeMask);

		const auto& GetDevice() const { return m_device; }

	private:

		static constexpr UINT k_firstAdapterIndex = 0U;

		TypeAlias::ComPtr<ID3D12Device9> m_device = nullptr;
	};
}