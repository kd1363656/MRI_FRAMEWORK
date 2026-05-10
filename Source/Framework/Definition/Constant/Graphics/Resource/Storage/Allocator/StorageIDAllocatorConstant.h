#pragma once

namespace FWK::Constant
{
	inline constexpr TypeAlias::StorageID k_invalidStorageID = std::numeric_limits<TypeAlias::StorageID>::max();

	inline constexpr TypeAlias::StorageID k_invalidStorageIDCapacity = 0U;
	
	inline constexpr TypeAlias::StorageID k_defaultCreateStorageIDCapacity = 3000U;
}