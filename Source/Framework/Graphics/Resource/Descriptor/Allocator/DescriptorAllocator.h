#pragma once

namespace FWK::Graphics
{
	class DescriptorAllocator
	{
	public:

		DescriptorAllocator()          = default;
		virtual ~DescriptorAllocator() = default;

		void Init();
		bool Create(const UINT a_capacity);

		UINT Allocate();

		void Release(const UINT a_index);

		bool IsAllocated(const UINT a_index) const;

		UINT GetCapacity() const { return m_capacity; }

	private:

		static constexpr UINT k_invalidDescriptorIndex = UINT_MAX;

		std::vector<bool> m_isAllocatedList = std::vector<bool>();
		std::queue<UINT>  m_freeIndexQueue  = std::queue<UINT>();

		UINT m_capacity = 0U;
		UINT m_nextIndex = 0U;
	};
}