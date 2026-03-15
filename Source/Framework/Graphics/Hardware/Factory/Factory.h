#pragma once

namespace FWK::Graphics
{
	class Factory final
	{
	public:

		Factory()  = default;
		~Factory() = default;

		void Init();
		bool Create();

		const auto& GetFactory() const { return m_factory; }

	private:

		Microsoft::WRL::ComPtr<IDXGIFactory7> m_factory = nullptr;
	};
}