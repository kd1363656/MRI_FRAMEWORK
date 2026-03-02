#pragma once

namespace FWK::Graphics
{
	class Device;
}

namespace FWK::Converter
{
	class DeviceJsonConverter final : public JsonConverterBase
	{
	public:

		explicit DeviceJsonConverter(Graphics::Device& a_deivce);
		~DeviceJsonConverter        ()							 override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()								 override;

	private:

		Graphics::Device& m_device;
	};
}