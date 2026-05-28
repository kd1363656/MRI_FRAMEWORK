#pragma once

namespace FWK::Graphics
{
	class ConstantBufferUploaderBase
	{
	public:

		         ConstantBufferUploaderBase();
		explicit ConstantBufferUploaderBase(const UINT64& a_constantBufferTypeSize);
		virtual ~ConstantBufferUploaderBase();

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     (const Device&         a_device);

		void BeginFrame();

		nlohmann::json Serialize() const;

		std::size_t AllocateCurrentBufferIndex();

		void SetCreateConstantBufferNUM(const UINT64& a_set) { m_createConstantBufferNUM = a_set; }

		const auto& GetREFCreateConstantBufferNUM() const { return m_createConstantBufferNUM; }

		const auto& GetREFUploadBuffer() const { return m_uploadBuffer; }

		auto& GetMutableREFUploadBuffer() { return m_uploadBuffer; }

	private:
		
		static constexpr UINT k_invalidBufferTypeSize = 0U;

		UploadBuffer m_uploadBuffer = UploadBuffer();

		Converter::ConstantBufferUploaderBaseJsonConverter m_constantBufferUploaderJsonConverter = {};

		UINT64 m_constantBufferTypeSize;
		UINT64 m_createConstantBufferNUM;

		FWK_DEFINE_TYPE_INFO_ROOT(ConstantBufferUploaderBase);
	};
}