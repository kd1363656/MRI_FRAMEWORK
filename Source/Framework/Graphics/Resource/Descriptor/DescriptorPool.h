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

			if (!m_descriptorHeapSlotAllocator.Create(m_descriptorCapacity))
			{
				assert(false && "ディスクリプタアロケータの作成処理に失敗しました。");
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
			return m_descriptorHeapSlotAllocator.Allocate();
		}

		void SetDescriptorCapacity(const UINT a_set) { m_descriptorCapacity = a_set; }

		auto FetchCPUHandle(const UINT a_index) const
		{
			return m_descriptorHeap.FetchCPUHandle(a_index);
		}

		auto GetVALDescriptorCapacity() const { return m_descriptorCapacity; }

	private:

		UINT m_descriptorCapacity = Constant::k_defaultDescriptorCapacity;

		Type						m_descriptorHeap			  = {};
		DescriptorHeapSlotAllocator m_descriptorHeapSlotAllocator = {};

		JsonConverter::DescriptorPoolJsonConverter<Type> m_descriptorPoolJsonConverter = {};
	};
}