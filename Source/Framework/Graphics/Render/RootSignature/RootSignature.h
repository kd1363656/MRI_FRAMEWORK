#pragma once

namespace FWK::Graphics
{
	class RootSignature final
	{
	public:

		explicit RootSignature(const Device& a_device);
		~RootSignature        ();

		void Init  ();
		bool Create();

		const auto& GetRootSignature() const { return m_rootSignature; }

	private:

		const Device& k_device;

		ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}