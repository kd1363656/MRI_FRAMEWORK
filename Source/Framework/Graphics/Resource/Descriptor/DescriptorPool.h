#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedDescriptorHeapBaseConcept Type>
	class DescriptorPool final
	{
	public:

		DescriptorPool () = default;
		~DescriptorPool() = default;

		void Deserialize(const nlohmann::json& a_rootJson)
		{
			if (a_rootJson.is_null()) { return; }
			m_descriptorPoolJsonConverter.Deserialize(a_rootJson, *this);
		}
		bool Create(const Device& a_device)
		{
			if (!m_descriptorHeap.Create(m_descriptorCapacity, a_device))
			{
				assert(false && "ディスクリプタヒープの作成処理に失敗しました。");
				return false;
			}

			if (!m_descriptorHeapIndexAllocator.Create(m_descriptorCapacity))
			{
				assert(false && "ディスクリプタヒープインデックスアロケータの作成処理に失敗しました。");
				return false;
			}

			return true;
		}

		auto Serialize() const
		{
			return m_descriptorPoolJsonConverter.Serialize(*this);
		}

		auto Allocate()
		{
			return m_descriptorHeapIndexAllocator.Allocate();
		}

		void SetDescriptorCapacity(const UINT a_set) { m_descriptorCapacity = a_set; }

		auto FetchVALCPUHandle(const UINT a_index) const
		{
			return m_descriptorHeap.FetchVALCPUHandle(a_index);
		}

		auto GetVALDescriptorCapacity() const { return m_descriptorCapacity; }

	private:

		UINT m_descriptorCapacity = Constant::k_defaultDescriptorCapacity;

		Type						 m_descriptorHeap			    = {};
		DescriptorHeapIndexAllocator m_descriptorHeapIndexAllocator = {};

		JsonConverter::DescriptorPoolJsonConverter<Type> m_descriptorPoolJsonConverter = {};
	};
}