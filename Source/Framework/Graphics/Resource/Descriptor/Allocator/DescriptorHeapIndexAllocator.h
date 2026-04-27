#pragma once

namespace FWK::Graphics
{
	class DescriptorHeapIndexAllocator final
	{
	public:

		 DescriptorHeapIndexAllocator() = default;
		~DescriptorHeapIndexAllocator() = default;

		bool Create(const UINT a_descriptorCapacity);

		void Release(const UINT a_index);

		UINT Allocate();

	private:

		bool IsValidIndex(const UINT a_index) const;

		static constexpr UINT k_firstDescriptorHeapIndex = 0U;
		
		static constexpr bool k_unallocatedDescriptorState = false;
		static constexpr bool k_allocatedDescriptorState   = true;

		UINT m_descriptorCapacity = Constant::k_invalidDescriptorCapacity;
		UINT m_nextIndex		  = k_firstDescriptorHeapIndex;

		std::vector<bool> m_isAllocatedList = {};

		std::queue<UINT> m_freeIndexQueue = {};
	};
}