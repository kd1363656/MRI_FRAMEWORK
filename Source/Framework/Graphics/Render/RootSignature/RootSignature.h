#pragma once

namespace FWK::Graphics
{
	class RootSignature final
	{
	private:

		using RootParameterIndexMap = std::unordered_map<TypeAlias::TypeTag, UINT>;

	public:
		
		 RootSignature() = default;
		~RootSignature() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create		(const Device&		   a_device);

		nlohmann::json Serialize() const;

		void SetRootSignatureFlags  (const D3D12_ROOT_SIGNATURE_FLAGS a_set) { m_rootSignatureFlags   = a_set; }
		void SetRootSignatureVersion(const D3D_ROOT_SIGNATURE_VERSION a_set) { m_rootSignatureVersion = a_set; }

		UINT FindVALRootParameterIndex(const TypeAlias::TypeTag a_tag) const;

		const auto& GetREFRootSignature() const { return m_rootSignature; }

		const auto& GetREFRootParameterIndexMap() const { return m_rootParameterIndexMap; }

		const auto& GetREFRootParameterRecordList() const { return m_rootParameterRecordList; }
		const auto& GetREFStaticSamplerDescList  () const { return m_staticSamplerDescList; }

		auto& GetMutableREFRootParameterIndexMap  () { return m_rootParameterIndexMap; }
		auto& GetMutableREFRootParameterRecordList() { return m_rootParameterRecordList; }
		auto& GetMutableREFStaticSamplerDescList  () { return m_staticSamplerDescList; }

		auto GetVALRootSignatureFlags  () const { return m_rootSignatureFlags; }
		auto GetVALRootSignatureVersion() const { return m_rootSignatureVersion; }

	private:

		TypeAlias::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

		D3D12_ROOT_SIGNATURE_FLAGS m_rootSignatureFlags   = {};
		D3D_ROOT_SIGNATURE_VERSION m_rootSignatureVersion = {};

		JsonConverter::RootSignatureJsonConverter m_rootSignatureJsonConverter = {};

		RootParameterIndexMap m_rootParameterIndexMap = {};

		std::vector<D3D12_STATIC_SAMPLER_DESC> m_staticSamplerDescList = {};

		// D3D12_ROOT_PARAMETERは、内部にポインタを持つため要素の再確保が発生した場合に
		// ポインタが無効になる可能性があるのでreserveするかemplace_backした後にポインタを渡す
		std::vector<Struct::RootParameterRecord> m_rootParameterRecordList = {};
	};
}