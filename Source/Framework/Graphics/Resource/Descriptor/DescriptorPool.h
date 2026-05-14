#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedDescriptorHeapBaseConcept Type>
	class DescriptorPool final
	{
	public:

		 DescriptorPool() = default;
		~DescriptorPool() = default;

		void Deserialize(const nlohmann::json& a_rootJson)
		{
			if (a_rootJson.is_null()) { return; }

			m_descriptorPoolJsonConverter.Deserialize(a_rootJson, *this);
		}
		bool Create(const Device& a_device)
		{
			if (!m_descriptorHeap.Create(a_device, m_storageIDCapacity))
			{
				assert(false && "ディスクリプタヒープの作成処理に失敗しました。");
				return false;
			}

			if (!m_storageIDAllocator.Create(m_storageIDCapacity))
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
			return m_storageIDAllocator.Allocate();
		}

		bool CopyCPUOnlyDescriptorToShaderVisibleDescriptor(const TypeAlias::StorageID a_storageID, const Device& a_device) const
		{
			return m_descriptorHeap.CopyCPUOnlyDescriptorToShaderVisibleDescriptor(a_storageID, a_device);
		}

		void Release(const TypeAlias::StorageID a_storageID)
		{
			m_storageIDAllocator.Release(a_storageID);
		}

		void SetDescriptorCapacity(const TypeAlias::StorageID a_set) { m_storageIDCapacity = a_set; }

		auto FetchShaderVisibleDescriptorHeap() const 
		{
			return m_descriptorHeap.FetchPTRShaderVisibleDescriptorHeap();
		}

		auto FetchVALCPUOnlyCPUHandle(const TypeAlias::StorageID a_storageID) const
		{
			return m_descriptorHeap.FetchVALCPUOnlyCPUHandle(a_storageID);
		}
		auto FetchVALShaderVisibleCPUHandle(const TypeAlias::StorageID a_storageID) const
		{
			return m_descriptorHeap.FetchVALShaderVisibleCPUHandle(a_storageID);
		}

		auto FetchVALShaderVisibleGPUHandle(const TypeAlias::StorageID a_storageID) const
		{
			return m_descriptorHeap.FetchVALShaderVisibleGPUHandle(a_storageID);
		}

		const auto& GetREFDescriptorHeap    () const { return m_descriptorHeap; }
		const auto& GetREFStorageIDAllocator() const { return m_storageIDAllocator; }

		auto& GetMutableREFStorageIDAllocator() { return m_storageIDAllocator; }

		auto GetVALStorageIDCapacity() const { return m_storageIDCapacity; }

	private:

		Type			   m_descriptorHeap		= {};
		StorageIDAllocator m_storageIDAllocator = {};

		Converter::DescriptorPoolJsonConverter<Type> m_descriptorPoolJsonConverter = {};

		TypeAlias::StorageID m_storageIDCapacity = Constant::k_defaultCreateStorageIDCapacity;
	};
}