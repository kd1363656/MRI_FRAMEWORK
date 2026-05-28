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

		template <typename ConstantBufferType>
		D3D12_GPU_VIRTUAL_ADDRESS Write(const ConstantBufferType& a_constantBuffer)
		{
			auto* const l_mappedData = m_uploadBuffer.Map();

			if (!l_mappedData)
			{
				assert(false && "定数バッファのMapに失敗しました。");
				return {};
			}

			const auto& l_constantBufferAlignedSize = Utility::Math::AlignUp(sizeof(ConstantBufferType), Constant::k_constantBufferAlignment);

			// 定数バッファのサイズがこの定数バッファアップローダーと一致しなければreturn
			if (Utility::Math::AlignUp(m_constantBufferTypeSize, Constant::k_constantBufferAlignment) != l_constantBufferAlignedSize)
			{
				assert(false && "定数バッファのアライメントサイズが一致しません。");
				return {};
			}

			// 現在未使用の定数バッファのインデックスを取得
			const auto& l_index = AllocateCurrentBufferIndex();

			const auto  l_constantBufferOffset = l_index * l_constantBufferAlignedSize;

			std::memcpy(l_mappedData + l_constantBufferOffset, &a_constantBuffer, sizeof(Struct::CBFinalPresent));

			return m_uploadBuffer.FetchVALGPUVirtualAddress() + l_constantBufferOffset;
		}

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