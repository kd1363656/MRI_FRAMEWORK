#pragma once

namespace FWK::Graphics
{
	class FrameResource final
	{
	private:

		using ConstantBufferMap = std::unordered_map<TypeAlias::StaticTypeID, std::shared_ptr<ConstantBufferBase>>;

	public:

		 FrameResource() = default;
		~FrameResource() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     (const Device&		   a_device);

		nlohmann::json Serialize() const;

		void AddConstantBuffer(const std::shared_ptr<ConstantBufferBase>& a_constantBuffer);

		const auto& GetREFConstantBufferMap() const { return m_constantBufferMap; }

		const auto& GetREFDirectCommandAllocator() const { return m_directCommandAllocator; }

		auto& GetMutableREFDirectCommandAllocator() { return m_directCommandAllocator; }

	private:

		ConstantBufferMap m_constantBufferMap = {};

		DirectCommandAllocator m_directCommandAllocator = {};

		JsonConverter::FrameResourceJsonConverter m_frameResourceJsonConverter = {};
	};
}