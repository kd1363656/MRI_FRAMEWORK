#pragma once

namespace FWK::Graphics
{
	class ConstantBufferBase;
}

namespace FWK::Converter
{
	class ConstantBufferBaseJsonConverter final
	{
	public:
		
		 ConstantBufferBaseJsonConverter() = default;
		~ConstantBufferBaseJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::ConstantBufferBase& a_constantBufferBase) const;

		nlohmann::json Serialize(const Graphics::ConstantBufferBase& a_constantBufferBase) const;

	private:

		static constexpr std::string_view k_createConstantBufferNUMJsonKey = "CreateConstantBufferNUM";
	};
}