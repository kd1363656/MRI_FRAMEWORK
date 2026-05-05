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
		bool Create     (const Device& a_device);

		nlohmann::json Serialize() const;

		void AddConstantBuffer(const std::shared_ptr<ConstantBufferBase>& a_constantBuffer);

		const auto& GetREFDirectCommandAllocator() const { return m_directCommandAllocator; }

		const auto& GetREFConstantBufferMap() const { return m_constantBufferMap; }

		auto& GetMutableREFDirectCommandAllocator() { return m_directCommandAllocator; }

	private:

		DirectCommandAllocator m_directCommandAllocator = {};

		ConstantBufferMap m_constantBufferMap = {};

		JsonConverter::FrameResourceJsonConverter m_frameResourceJsonConverter = {};
	};
}