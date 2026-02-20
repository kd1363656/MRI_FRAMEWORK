#pragma once

namespace FWK
{
	class FPSController;
}

namespace FWK::Converter
{
	class FPSControllerJsonConverter final : public JsonConverterBase
	{
	public:

		explicit FPSControllerJsonConverter(FPSController& a_fpsController);
		~FPSControllerJsonConverter        ()							    override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()                                 override;

	private:

		FPSController& m_fpsController;
	};
}