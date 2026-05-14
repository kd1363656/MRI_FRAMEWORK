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
		bool Create     (const TypeAlias::StorageID a_storageIDCapacity);

		void Release(const TypeAlias::StorageID a_storageID);

		nlohmann::json Serialize() const;

		TypeAlias::StorageID Allocate();

	private:

		bool IsValidStorageID(const TypeAlias::StorageID a_storageID) const;

		static constexpr TypeAlias::StorageID k_initialNextStorageID = 0U;

		static constexpr bool k_unallocatedStorageIDState = false;
		static constexpr bool k_allocatedStorageIDState   = true;

		std::vector<bool> m_isAllocatedList = {};

		std::queue<TypeAlias::StorageID> m_freeStorageIDQueue = {};

		TypeAlias::StorageID m_storageIDCapacity = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_nextStorageID     = k_initialNextStorageID;
	};
}