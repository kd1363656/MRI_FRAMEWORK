#pragma once

namespace FWK::Graphics
{
	class GPUMemoryAllocator final
	{
	public:

		 GPUMemoryAllocator() = default;
		~GPUMemoryAllocator() = default;

		bool Create(const Device& a_device);

		bool CreateTextureResource(const D3D12_RESOURCE_DESC&                    a_resourceDesc,
								   const D3D12_CLEAR_VALUE*	                     a_clearValue,
								   const D3D12_RESOURCE_STATES                   a_initialResourceState,
										 TypeAlias::ComPtr<ID3D12Resource2>&     a_textureResource,
										 TypeAlias::ComPtr<D3D12MA::Allocation>& a_allocation) const;

		const auto& GetREFAllocator() const { return m_allocator; }

	private:

		TypeAlias::ComPtr<D3D12MA::Allocator> m_allocator = nullptr;
	};
}