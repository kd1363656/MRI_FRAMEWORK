#pragma once

namespace FWK::Graphics
{
	class HeapPage
	{
	public:

		 HeapPage() = default;
		~HeapPage() = default;

		bool Create(const UINT64& a_heapSize, const Device& a_device, const TypeAlias::HeapPageID a_heapPageID);

		bool Allocate(const UINT64& a_allocationSize, const UINT64& a_alignment, UINT64& a_heapOffset);

		bool Release(const UINT64& a_heapOffset, const UINT64& a_allocationSize);

		const auto& GetREFTextureHeap() const { return m_textureHeap; }

		const auto& GetREFHeapSize() const { return m_heapSize; }

		auto GetVALTextureHeapPage() const { return m_heapPageID; }

	private:

		TypeAlias::ComPtr<ID3D12Heap> m_textureHeap = nullptr;

		UINT64 m_heapSize = 0ULL;

		TypeAlias::HeapPageID m_heapPageID = Constant::k_invalidHeapPageID;
	};
}