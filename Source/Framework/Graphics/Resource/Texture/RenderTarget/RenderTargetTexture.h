#pragma once

namespace FWK::Graphics
{
	class RenderTargetTexture final
	{
	public:

		 RenderTargetTexture() = default;
		~RenderTargetTexture() = default;

		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Device&							 a_device, 
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		nlohmann::json Serialize() const;

		void SetClearColor(const TypeAlias::Math::Color& a_set) { m_clearColor = a_set; }

		void SetCurrentResourceState(const D3D12_RESOURCE_STATES a_set) { m_currentResourceState = a_set; }

		void SetFormat(const DXGI_FORMAT a_set) { m_format = a_set; }

		void SetWidth (const UINT a_set) { m_width  = a_set; }
		void SetHeight(const UINT a_set) { m_height = a_set; }

		const auto& GetClearColor() const { return m_clearColor; }

		const auto& GetREFGPUResource() const { return m_gpuResource; }

		auto GetVALCurrentResourceState() const { return m_currentResourceState; }

		auto GetFormat() const { return m_format; }

		auto GetWidth () const { return m_width; }
		auto GetHeight() const { return m_height; }

		auto GetVALRTVStorageID() const { return m_rtvStorageID; }
		auto GetVALSRVStorageID() const { return m_srvStorageID; }

	private:

		bool CreateRenderTargetView(const Device& a_device, DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool);

		bool CreateShaderResourceView(const Device& a_device, DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);

		static constexpr UINT k_clearValueIndexR = 0U;
		static constexpr UINT k_clearValueIndexG = 1U;
		static constexpr UINT k_clearValueIndexB = 2U;
		static constexpr UINT k_clearValueIndexA = 3U;

		Converter::RenderTargetTextureJsonConverter m_renderTargetTextureJsonConverter = {};

		TypeAlias::Math::Color m_clearColor = Constant::k_renderTargetDefaultClearColor;

		Struct::GPUResource m_gpuResource = {};

		D3D12_RESOURCE_STATES m_currentResourceState = Constant::k_defaultRenderTargetTextureResourceState;

		DXGI_FORMAT m_format = Constant::k_defaultSwapChainBackBufferFormat;

		UINT m_width  = Constant::k_defaultRenderTextureWidth;
		UINT m_height = Constant::k_defaultRenderTextureHeight;

		TypeAlias::StorageID m_rtvStorageID = Constant::k_invalidStorageID;
		TypeAlias::StorageID m_srvStorageID = Constant::k_invalidStorageID;
	};
}