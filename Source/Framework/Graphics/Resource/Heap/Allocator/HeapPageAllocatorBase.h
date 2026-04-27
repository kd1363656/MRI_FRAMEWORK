#pragma once

namespace FWK::Graphics
{
	class HeapPageAllocatorBase
	{
	public:

		explicit HeapPageAllocatorBase(const D3D12_HEAP_TYPE a_createHeapType, const D3D12_HEAP_FLAGS a_createHeapFlags);
		virtual ~HeapPageAllocatorBase();

		bool Allocate(const UINT64&		           a_allocationSize,
					  const UINT64&		           a_alignment,
					  const Device&		           a_device,
						    UINT64&		           a_heapOffset,
							TypeAlias::HeapPageID& a_heapPageID);

		bool Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize, TypeAlias::HeapPageID a_heapPageID);

		HeapPage* FindMutablePTRHeapPage(const TypeAlias::HeapPageID a_heapPageID);

	private:

		bool CreateHeapPage(const Device& a_device, const UINT64& a_allocationSize, const UINT64& a_alignment);

		TypeAlias::HeapPageID GenerateHeapPageID();

		static constexpr UINT64 k_defaultHeapPageSize = 64ULL * 1024ULL * 1024ULL;

		static constexpr UINT k_initialOffset = 0U;

		static constexpr TypeAlias::HeapPageID k_initialHeapPageID = 0U;

		const D3D12_HEAP_TYPE  k_createHeapType;
		const D3D12_HEAP_FLAGS k_createHeapFlags;

		UINT64 m_heapPageSize = k_defaultHeapPageSize;

		TypeAlias::HeapPageID m_nextHeapPageID = k_initialHeapPageID;

		std::vector<HeapPage> m_heapPageList = {};
	};
}