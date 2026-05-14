#pragma once

namespace FWK::Graphics
{
	// ※注意
	// StorageIDを配列のインデックスとしても使用します
	class StorageIDAllocator final
	{
	public:

		 StorageIDAllocator() = default;
		~StorageIDAllocator() = default;

		void Deserialize(const nlohmann::json&      a_rootJson);
		bool Create     ();

		nlohmann::json Serialize() const;

		void Release(const TypeAlias::StorageID a_storageID);

		TypeAlias::StorageID Allocate();

		void SetStorageIDCapacity(TypeAlias::StorageID a_set) { m_storageIDCapacity = a_set; }

		auto GetVALStorageIDCapacity() const { return m_storageIDCapacity; }

	private:

		bool IsValidStorageID(const TypeAlias::StorageID a_storageID) const;

		static constexpr TypeAlias::StorageID k_initialNextStorageID = 0U;

		static constexpr bool k_unallocatedStorageIDState = false;
		static constexpr bool k_allocatedStorageIDState   = true;

		std::vector<bool> m_isAllocatedList = {};

		std::queue<TypeAlias::StorageID> m_freeStorageIDQueue = {};

		Converter::StorageIDAllocatorJsonConverter m_storageAllocatorJsonConverter = {};

		TypeAlias::StorageID m_storageIDCapacity = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_nextStorageID     = k_initialNextStorageID;
	};
}