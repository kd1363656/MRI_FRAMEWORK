#pragma once

namespace FWK::Graphcis
{
	class RootSignature;
}

namespace FWK::JsonConverter
{
	class RootSignatureJsonConverter final
	{
	public:

		 RootSignatureJsonConverter() = default;
		~RootSignatureJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const;

		nlohmann::json Serialize(const Graphics::RootSignature& a_rootSignature) const;
	};
}