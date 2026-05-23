#pragma once

namespace FWK::Graphics
{
	class SceneTexture final
	{
	public:

		 SceneTexture() = default;
		~SceneTexture() = default;

		bool Create(const Device&			  a_device, 
					const GPUMemoryAllocator& a_gpuMemoryAllocator,
					const UINT a_width,
					const UINT a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

		bool IsValid() const;

		std::weak_ptr<RenderTargetTexture> GetVAKSceneColorTexture		 () const { return m_sceneColorTexture; }
		std::weak_ptr<DepthStencilTexture> GetVAKSceneDepthStencilTexture() const { return m_sceneDepthStencilTexture; }
		
	private:

		static constexpr DXGI_FORMAT k_defaultSceneColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		std::shared_ptr<RenderTargetTexture> m_sceneColorTexture		= nullptr;
		std::shared_ptr<DepthStencilTexture> m_sceneDepthStencilTexture = nullptr;
	};
}