#pragma once

namespace FWK::Converter
{
	class JsonConverterBase
	{
	public:

		JsonConverterBase         () = default;
		virtual ~JsonConverterBase() = default;

		virtual void           Deserialize(const nlohmann::json& a_rootJson) = 0;
		virtual nlohmann::json Serialize  ()  							     = 0;
	};
}