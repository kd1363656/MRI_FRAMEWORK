#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture final
	{
	public:
	
		 DepthStencilTexture() = default;
		~DepthStencilTexture() = default;
	
		bool Create(const Graphics::Device&   a_device,
					const GPUMemoryAllocator& a_gpuMemoryAllocator,
					const UINT a_width,
					const UINT a_height,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);
	
		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALDSVStorageID() const { return m_dsvStorageID; }

	private:
	
		static constexpr float k_clearDepth = 1.0F;

		static constexpr UINT k_depthStencilSampleCount   = 1U;
		static constexpr UINT k_depthStencilSampleQuality = 0U;

		static constexpr UINT16 k_depthStencilArraySize = 1U;
		static constexpr UINT16 k_depthStencilMIPLevels = 1U;

		static constexpr UINT8 k_clearStencil = 0U;

		Struct::GPUResource m_gpuResource = {};

		TypeAlias::StorageID m_dsvStorageID = Constant::k_invalidStorageID;
	};
}