#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture final
	{
	public:
	
		 DepthStencilTexture() = default;
		~DepthStencilTexture() = default;
	
		void Deserialize		  (const nlohmann::json& a_rootJson);
		void ApplyWindowSizeIfNeed(const Struct::WindowCONFIG& a_windowConfig);

		bool Create(const Graphics::Device& a_device, const GPUMemoryAllocator& a_gpuMemoryAllocator, DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);
	
		nlohmann::json Serialize() const;

		void SetCurrentResourceState(const D3D12_RESOURCE_STATES a_set) { m_currentResourceState = a_set; }

		void SetFormat(const DXGI_FORMAT a_set) { m_format = a_set; }

		void SetWidth (const UINT a_set) { m_width  = a_set; }
		void SetHeight(const UINT a_set) { m_height = a_set; }

		void SetIsUseWindowSize(const bool a_set) { m_isUseWindowSize = a_set; }

		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALCurrentResourceState() const { return m_currentResourceState; }

		auto GetFormat() const { return m_format; }

		auto GetVALDSVStorageID() const { return m_dsvStorageID; }

		UINT GetWidth () const { return m_width; }
		UINT GetHeight() const { return m_height; }

		bool GetIsUseWindowSize() const { return m_isUseWindowSize; }

	private:
	
		Converter::DepthStencilTextureJsonConverter m_depthStencilTextureJsonConverter = {};

		Struct::GPUResource m_gpuResource = {};

		D3D12_RESOURCE_STATES m_currentResourceState = Constant::k_defaultDepthStencilTextureResourceState;

		DXGI_FORMAT m_format = Constant::k_defaultDepthStencilTextureFormat;

		TypeAlias::StorageID m_dsvStorageID = Constant::k_invalidStorageID;

		UINT m_width  = Constant::k_defaultDepthStencilTextureWidth;
		UINT m_height = Constant::k_defaultDepthStencilTextureHeight;

		bool m_isUseWindowSize = false;
	};
}