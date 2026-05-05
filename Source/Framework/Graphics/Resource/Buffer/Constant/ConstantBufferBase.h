#pragma once

namespace FWK::Graphics
{
	class ConstantBufferBase
	{
	public:

		         ConstantBufferBase();
		explicit ConstantBufferBase(const UINT64& a_constantBufferTypeSize);
		virtual ~ConstantBufferBase();

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     (const Device&         a_device);

		nlohmann::json Serialize() const;

		void SetCreateConstantBufferNUM(const UINT64& a_set) { m_createConstantBufferNUM = a_set; }

		const auto& GetREFCreateConstantBufferNUM() const { return m_createConstantBufferNUM; }

		const auto& GetREFUploadConstantBuffer() const { return m_uploadConstantBuffer; }

	private:
		
		static constexpr UINT k_invalidBufferTypeSize = 0U;

		UploadBuffer m_uploadConstantBuffer = UploadBuffer();

		JsonConverter::ConstantBufferBaseJsonConverter m_constantBufferJsonConverter = {};

		UINT64 m_constantBufferTypeSize;
		UINT64 m_createConstantBufferNUM;

		FWK_DEFINE_TYPE_INFO_ROOT(ConstantBufferBase);
	};
}