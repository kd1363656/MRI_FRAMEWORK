#pragma once

namespace FWK::Render
{
	class Hardware
	{
	public:

		Hardware ();
		~Hardware();

		void Init          ();
		bool Create        ();
		void PostCreateInit(const HWND& a_hWND) const;

		void           Deserialize(const nlohmann::json& a_rootJson);
		nlohmann::json Serialize  ();

		const auto& GetFactory() const { return m_factory; }
		const auto& GetDevice () const { return m_device; }

		UINT GetGPUNodeMask() const { return m_gpuNodeMask; }

		void SetGPUNodeMask(const UINT a_set) { m_gpuNodeMask = a_set; }

	private:

		bool CreateFactory();
		bool CreateDevice ();

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		TypeDEF::ComPtr<IDXGIFactory7> m_factory;
		TypeDEF::ComPtr<ID3D12Device9> m_device;

		Converter::HardwareJsonConverter m_hardwareJsonConverter;

		UINT m_gpuNodeMask;
	};
}