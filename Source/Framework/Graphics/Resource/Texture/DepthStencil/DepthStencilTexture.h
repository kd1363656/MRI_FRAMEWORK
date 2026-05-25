#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture final
	{
	public:
	
		 DepthStencilTexture() = default;
		~DepthStencilTexture() = default;
	
		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Graphics::Device&   a_device,
					const GPUMemoryAllocator& a_gpuMemoryAllocator,
					const UINT a_width,
					const UINT a_height,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);
	
		nlohmann::json Serialize() const;

		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALDSVStorageID() const { return m_dsvStorageID; }

	private:
	
		Converter::DepthStencilTextureJsonConverter m_depthStencilTextureJsonConverter = {};

		Struct::GPUResource m_gpuResource = {};

		TypeAlias::StorageID m_dsvStorageID = Constant::k_invalidStorageID;
	};
}