#pragma once

namespace FWK::Graphics
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

	private:

		void DeserializeRootParameterIndexMap(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const;
		void DeserializeRootParameterList    (const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const;
		void DeserializeStaticSamplerDescList(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const;

		nlohmann::json SerializeRootParameterIndexMap(const Graphics::RootSignature& a_rootSignature) const;
		nlohmann::json SerializeRootParameterList    (const Graphics::RootSignature& a_rootSignature) const;
		nlohmann::json SerializeStaticSamplerDescList(const Graphics::RootSignature& a_rootSignature) const;

		void DeserializeDescriptorRangeList(const nlohmann::json& a_rootJson, Struct::RootParameter& a_rootParameter) const;

		nlohmann::json SerializeDescriptorRangeList(const Struct::RootParameter& a_rootParameter) const;
	};
}