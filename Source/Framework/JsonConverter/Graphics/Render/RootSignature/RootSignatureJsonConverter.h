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

		void DeserializeDescriptorRangeList(const nlohmann::json& a_rootJson, Struct::RootParameterRecord& a_rootParameterRecord) const;

		nlohmann::json SerializeDescriptorRangeList(const Struct::RootParameterRecord& a_rootParameterRecord) const;

		static constexpr FLOAT k_defaultStaticSamplerMIPLODBias = 0.0F;
		static constexpr FLOAT k_defaultMINLOD				    = 0.0F;

		static constexpr UINT k_defaultRootConstantsNum32BitValues = 0U;

		static constexpr UINT  k_defaultStaticSamplerMAXAnisotropy = 1U;

		static constexpr UINT k_defaultShaderRegister = 0U;
		static constexpr UINT k_defaultRegisterSpace  = 0U;

		static constexpr UINT k_defaultDescriptorRangeNUMDescriptors = 0U;
		static constexpr UINT k_defaultBaseShaderRegister            = 0U;
		static constexpr UINT k_invalidNUMDescriptorRange            = 0U;
	};
}