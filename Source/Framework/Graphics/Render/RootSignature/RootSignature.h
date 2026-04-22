#pragma once

namespace FWK::Graphics
{
	class RootSignature final
	{
	public:
		
		 RootSignature() = default;
		~RootSignature() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create		(const Device&		   a_device);

		nlohmann::json Serialize();

	private:

		D3D12_ROOT_SIGNATURE_FLAGS m_rootSignatureFlags   = {};
		D3D_ROOT_SIGNATURE_VERSION m_rootSignatureVersion = {};

		JsonConverter::RootSignatureJsonConverter m_rootSignatureJsonConverter = {};
	};
}