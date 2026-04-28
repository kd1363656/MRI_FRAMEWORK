#pragma once

namespace FWK::Graphics
{
	class PlacedResourceAllocatorBase
	{
	public:

		explicit PlacedResourceAllocatorBase(const D3D12_HEAP_TYPE a_createHeapType, const D3D12_HEAP_FLAGS a_createHeapFlags);
		virtual ~PlacedResourceAllocatorBase();

		bool CreatePlacedResource(const D3D12_RESOURCE_DESC&                    a_resourceDesc,
								  const Device&				                    a_device,
								  const D3D12_CLEAR_VALUE*                      a_clearValue,
								  const D3D12_RESOURCE_STATES                   a_initialResourceState,
										TypeAlias::ComPtr<ID3D12Resource2>&     a_resource,
									    Struct::PlacedResourceAllocationRecord& a_allocationRecord);

		bool Allocate(const UINT64&		           a_allocationSize,
					  const UINT64&		           a_alignment,
					  const Device&		           a_device,
						    UINT64&		           a_heapOffset,
							TypeAlias::HeapPageID& a_heapPageID);

		bool Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize, TypeAlias::HeapPageID a_heapPageID);

		const HeapPage* FindPTRHeapPage(const TypeAlias::HeapPageID a_heapPageID) const;

		HeapPage* FindMutablePTRHeapPage(const TypeAlias::HeapPageID a_heapPageID);

	private:

		bool CreateHeapPage(const Device& a_device, const UINT64& a_allocationSize, const UINT64& a_alignment);

		TypeAlias::HeapPageID GenerateHeapPageID();

		static constexpr UINT64 k_defaultHeapPageSize        = 64ULL * 1024ULL * 1024ULL;
		static constexpr UINT64 k_invalidHeapPageSize        = 0ULL;
		static constexpr UINT64 k_alignmentPaddingAdjustment = 1ULL;

		static constexpr TypeAlias::HeapPageID k_initialHeapPageID = 0U;

		const D3D12_HEAP_TYPE  k_createHeapType;
		const D3D12_HEAP_FLAGS k_createHeapFlags;

		UINT64 m_heapPageSize = k_defaultHeapPageSize;

		TypeAlias::HeapPageID m_nextHeapPageID = k_initialHeapPageID;

		std::vector<HeapPage> m_heapPageList = {};
	};
}