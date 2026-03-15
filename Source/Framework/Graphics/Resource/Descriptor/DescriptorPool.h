#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedDescriptorHeapBase DescriptorHeap>
	class DescriptorPool final
	{
	public:

		DescriptorPool()  = default;
		~DescriptorPool() = default;

		void Init()
		{
			m_descriptorHeap.Init();
			m_descriptorAllocator.Init();
		}

		bool Create(const Device& a_device)
		{
			m_descriptorHeap.Create(a_device);
			m_descriptorAllocator.Create(m_descriptorCapacityNum);
		}

	private:

		UINT m_descriptorCapacityNum;

		DescriptorHeap      m_descriptorHeap;
		DescriptorAllocator m_descriptorAllocator;
	};
}