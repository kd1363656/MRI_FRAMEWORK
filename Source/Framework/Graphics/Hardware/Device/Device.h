#pragma once

namespace FWK::Graphics
{
	class Device final
	{
	public:

		Device (const Factory& a_factory);
		~Device();

		void Init  ();
		bool Create();
		
		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize  ();

		const auto& GetDevice() const { return m_device; }

		UINT GetGPUNodeMask() const { return m_gpuNodeMask; }

		void SetGPUNodeMask(const UINT a_set) { m_gpuNodeMask = a_set; }

	private:

		const Factory& k_factory;

		ComPtr<ID3D12Device9> m_device;

		Converter::DeviceJsonConverter m_deviceJsonConverter;

		UINT m_gpuNodeMask;
	};
}