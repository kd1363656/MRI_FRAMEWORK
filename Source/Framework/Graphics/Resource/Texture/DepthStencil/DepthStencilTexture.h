#pragma once

namespace FWK::Graphics
{
	class DepthStencilTexture final
	{
	public:
	
		 DepthStencilTexture() = default;
		~DepthStencilTexture() = default;
	
		DepthStencilTexture(const DepthStencilTexture&)			  = delete;
		DepthStencilTexture(	  DepthStencilTexture&&) noexcept = default;
		
		DepthStencilTexture& operator=(const DepthStencilTexture&)			 = delete;
		DepthStencilTexture& operator=(		 DepthStencilTexture&&) noexcept = default;

		void Deserialize		  (const nlohmann::json& a_rootJson);
		void ApplyWindowSizeIfNeed(const Struct::WindowCONFIG& a_windowConfig);

		bool Create(const Graphics::Device& a_device, const GPUMemoryAllocator& a_gpuMemoryAllocator, DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);
	
		nlohmann::json Serialize() const;

		bool Resize(const Device&							 a_device, 
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const Struct::ClientSize&				 a_clientSize,
					const UINT64&							 a_retiredFenceValue,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool,
						  DeferredResourceReleaseQueue&      a_deferredResourceReleaseQueue);

		void SetCurrentResourceState(const D3D12_RESOURCE_STATES a_set) { m_currentResourceState = a_set; }

		void SetFormat(const DXGI_FORMAT a_set) { m_format = a_set; }

		void SetWidth (const UINT a_set) { m_width  = a_set; }
		void SetHeight(const UINT a_set) { m_height = a_set; }

		void SetIsUseWindowSize(const bool a_set) { m_isUseWindowSize = a_set; }

		static constexpr auto GetVALDefaultDepthStencilTextureFormat() { return k_defaultDepthStencilTextureFormat; }

		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALCurrentResourceState() const { return m_currentResourceState; }

		auto GetFormat() const { return m_format; }

		auto GetVALDSVStorageID() const { return m_dsvStorageID; }

		UINT GetWidth () const { return m_width; }
		UINT GetHeight() const { return m_height; }

		bool GetIsUseWindowSize() const { return m_isUseWindowSize; }

	private:
	
		bool IsValidTextureSize(const Struct::ClientSize& a_clientSize) const;

		bool IsSameTextureSize(const Struct::ClientSize& a_clientSize) const;

		bool IsValidCurrentResourceForDeferredRelease(const UINT64& a_retiredFenceValue) const;

		bool PushCurrentResourceForDeferredRelease(const UINT64& a_retiredFenceValue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue);

		static constexpr DXGI_FORMAT k_defaultDepthStencilTextureFormat = DXGI_FORMAT_D32_FLOAT;

		Converter::DepthStencilTextureJsonConverter m_depthStencilTextureJsonConverter = {};

		Struct::GPUResource m_gpuResource = {};

		D3D12_RESOURCE_STATES m_currentResourceState = Constant::k_defaultDepthStencilTextureResourceState;

		DXGI_FORMAT m_format = k_defaultDepthStencilTextureFormat;

		TypeAlias::StorageID m_dsvStorageID = Constant::k_invalidStorageID;

		UINT m_width  = Constant::k_invalidDepthStencilTextureWidth;
		UINT m_height = Constant::k_invalidDepthStencilTextureHeight;

		bool m_isUseWindowSize = false;
	};
}