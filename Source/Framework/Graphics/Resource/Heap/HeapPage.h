#pragma once

namespace FWK::Graphics
{
	class HeapPage final
	{
	public:

		 HeapPage() = default;
		~HeapPage() = default;

		bool Create(const UINT64&		        a_heapSize,
					const D3D12_HEAP_TYPE       a_heapType,
					const D3D12_HEAP_FLAGS      a_heapFlags,
					const Device&		        a_device,
					const TypeAlias::HeapPageID a_heapPageID);

		bool Allocate(const UINT64& a_allocationSize, const UINT64& a_alignment, UINT64& a_heapOffset);

		bool Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize);

		const auto& GetREFHeap() const { return m_heap; }

		const auto& GetREFHeapSize() const { return m_heapSize; }

		auto GetVALHeapPageID() const { return m_heapPageID; }

	private:

		bool HasOverlappingFreeBlock(const UINT64& a_beginOffset, const UINT64& a_freeSize) const;

		void MergeFreeBlocks();

		static constexpr std::size_t k_mergeStartIndex = 1ULL;

		TypeAlias::ComPtr<ID3D12Heap> m_heap = nullptr;

		UINT64 m_heapSize = 0ULL;

		TypeAlias::HeapPageID m_heapPageID = Constant::k_invalidHeapPageID;

		std::vector<Struct::FreeHeapBlock> m_freeHeapBlockList = {};
	};
}