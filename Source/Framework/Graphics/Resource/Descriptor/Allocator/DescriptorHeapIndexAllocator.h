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

		static constexpr UINT k_invalidDescriptorCapacity = 0U;
		static constexpr UINT k_initialNextIndex		  = 0U;

		UINT m_descriptorCapacity = 0U;
		UINT m_nextIndex		  = 0U;

		std::vector<bool> m_isAllocatedList = {};

		std::queue<UINT> m_freeIndexQueue = {};
	};
}