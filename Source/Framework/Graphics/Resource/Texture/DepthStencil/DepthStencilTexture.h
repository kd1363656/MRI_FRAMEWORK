#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture final
	{
	public:
	
		 DepthStencilTexture() = default;
		~DepthStencilTexture() = default;
	
		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Graphics::Device& a_device, const GPUMemoryAllocator& a_gpuMemoryAllocator, DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);
	
		nlohmann::json Serialize() const;

		void SetWidth (const UINT a_set) { m_width  = a_set; }
		void SetHeight(const UINT a_set) { m_height = a_set; }

		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALDSVStorageID() const { return m_dsvStorageID; }

		UINT GetWidth () const { return m_width; };
		UINT GetHeight() const { return m_height; };

	private:
	
		Converter::DepthStencilTextureJsonConverter m_depthStencilTextureJsonConverter = {};

		Struct::GPUResource m_gpuResource = {};

		TypeAlias::StorageID m_dsvStorageID = Constant::k_invalidStorageID;

		UINT m_width  = Constant::k_defaultDepthStencilTextureWidth;
		UINT m_height = Constant::k_defaultDepthStencilTextureHeight;
	};
}