#pragma once

namespace FWK::Graphics
{
	class LightSystem final
	{
	public:

		 LightSystem() = default;
		~LightSystem() = default;

		void ApplyDefaultSettings();

		void SetToConstantBuffer();

	private:

		std::shared_ptr<Struct::CBLightPass> m_cbLightPass = nullptr;
	};
}