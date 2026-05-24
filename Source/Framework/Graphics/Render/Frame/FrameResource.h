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

		void Init       ();
		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Device&							 a_device,
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const UINT								 a_width,
					const UINT								 a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

		nlohmann::json Serialize() const;

		void AddConstantBuffer(const std::shared_ptr<ConstantBufferBase>& a_constantBuffer);

		template <typename Type>
		std::weak_ptr<ConstantBufferBase> FindPTRConstantBuffer() const
		{
			const auto& l_itr = m_constantBufferMap.find(Type::GetTypeINFO().k_staticTypeID);

			if (l_itr == m_constantBufferMap.end()) { return std::weak_ptr<ConstantBufferBase>(); }

			if (!l_itr->second) { return std::weak_ptr<ConstantBufferBase>(); }

			return l_itr->second;
		}
		
		const auto& GetREFConstantBufferMap() const { return m_constantBufferMap; }

		const auto& GetREFDirectCommandAllocator() const { return m_directCommandAllocator; }
		const auto& GetREFSceneTexture			() const { return m_sceneTexture; }

		auto& GetMutableREFDirectCommandAllocator() { return m_directCommandAllocator; }

	private:

		std::shared_ptr<DirectCommandAllocator> m_directCommandAllocator = nullptr;

		ConstantBufferMap m_constantBufferMap = {};

		SceneTexture m_sceneTexture = {};

		Converter::FrameResourceJsonConverter m_frameResourceJsonConverter = {};
	};
}