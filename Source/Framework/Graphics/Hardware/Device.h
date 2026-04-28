#pragma once

namespace FWK::Graphics
{
	class Device final
	{
	public:

		 Device() = default;
		~Device() = default;

		bool Create(const Factory& a_factory);

		const auto& GetREFAdapter() const { return m_adapter; }
		const auto& GetREFDevice () const { return m_device; }

	private:

		static constexpr DXGI_GPU_PREFERENCE k_defaultGPUPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		static constexpr DXGI_ADAPTER_FLAG3  k_excludedAdapterFlag  = DXGI_ADAPTER_FLAG3_SOFTWARE;

		static constexpr UINT k_firstAdapterIndex = 0U;

		static constexpr auto k_preferredFeatureLevelList =std::to_array<D3D_FEATURE_LEVEL>(
		{
			D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		});

#if defined(_DEBUG)
		static constexpr std::wstring_view k_deviceDebugLogSeparator      = L"\n===================================================================\n";
		static constexpr std::wstring_view k_selectedGPUNameDebugLogLabel = L"使用GPU : ";
#endif

		TypeAlias::ComPtr<IDXGIAdapter4> m_adapter = nullptr;
		TypeAlias::ComPtr<ID3D12Device9> m_device  = nullptr;
	};
}