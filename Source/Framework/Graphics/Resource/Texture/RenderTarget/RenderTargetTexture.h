#pragma once

namespace FWK::Graphics
{
	class RenderTargetTexture final
	{
	public:

		 RenderTargetTexture() = default;
		~RenderTargetTexture() = default;

		bool Create(const Device&			  a_device,
				    const GPUMemoryAllocator& a_gpuMemoryAllocator,	
					const DXGI_FORMAT		  a_format,
					const UINT				  a_width,
					const UINT				  a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		void SetCurrentResourceState(const D3D12_RESOURCE_STATES a_set) { m_currentResourceState = a_set; }

		const auto& GetClearColor() const { return m_clearColor; }

		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALCurrentResourceState() const { return m_currentResourceState; }

		auto GetVALRTVStorageID() const { return m_rtvStorageID; }
		auto GetVALSRVStorageID() const { return m_srvStorageID; }

	private:

		bool CreateRenderTargetView(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		bool CreateShaderResourceView(const Device& a_device, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		static constexpr UINT k_clearValueIndexR = 0U;
		static constexpr UINT k_clearValueIndexG = 1U;
		static constexpr UINT k_clearValueIndexB = 2U;
		static constexpr UINT k_clearValueIndexA = 3U;

		TypeAlias::Math::Color m_clearColor = Constant::k_renderTargetDefaultClearColor;

		Struct::GPUResource m_gpuResource = {};

		D3D12_RESOURCE_STATES m_currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

		DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;

		UINT m_width  = 0U;
		UINT m_height = 0U;

		TypeAlias::StorageID m_rtvStorageID = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_srvStorageID = Constant::k_invalidStorageID;
	};
}