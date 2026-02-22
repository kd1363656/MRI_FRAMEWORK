#pragma once

namespace FWK::Render
{
	class Hardware;
}

namespace FWK::Converter
{
	class HardwareJsonConverter final : public JsonConverterBase
	{
	public:

		explicit HardwareJsonConverter(Render::Hardware& a_hardware);
		~HardwareJsonConverter        ()							 override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()								 override;

	private:

		Render::Hardware& m_hardware;
	};
}