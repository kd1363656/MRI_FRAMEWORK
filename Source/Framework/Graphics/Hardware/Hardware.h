#pragma once

namespace FWK::Graphics
{
	class Hardware final
	{
	public:

		Hardware()  = default;
		~Hardware() = default;

		void Init();
		bool Create();

		const auto& GetFactory() const { return m_factory; }
		const auto& GetDevice()  const { return m_device; }

	private:

		Factory m_factory = Factory();
		Device  m_device  = Device();
	};
}