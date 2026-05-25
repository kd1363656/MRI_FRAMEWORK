#pragma once

namespace FWK::Graphics
{
	class SceneTexture final
	{
	public:

		 SceneTexture() = default;
		~SceneTexture() = default;

		void INIT();

		void Deserialize(const nlohmann::json& a_rootJson);

		bool Create(const Device&							 a_device, 
					const GPUMemoryAllocator&				 a_gpuMemoryAllocator,
					const UINT								 a_width,
					const UINT								 a_height,
						  DescriptorPool<RTVDescriptorHeap>& a_rtvDescriptorPool,
						  DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool,
						  DescriptorPool<DSVDescriptorHeap>& a_dsvDescriptorPool);

		nlohmann::json Serialize() const;

		std::weak_ptr<RenderTargetTexture> GetVALFinalSceneTexture			  () const { return m_finalSceneTexture; }
		std::weak_ptr<DepthStencilTexture> GetVALFinalSceneDepthStencilTexture() const { return m_finalSceneDepthStencilTexture; }
		
	private:

		static constexpr DXGI_FORMAT k_defaultSceneColorFormat = Constant::k_defaultSwapChainBackBufferFormat;

		std::shared_ptr<RenderTargetTexture> m_finalSceneTexture		     = nullptr;
		std::shared_ptr<DepthStencilTexture> m_finalSceneDepthStencilTexture = nullptr;

		Converter::SceneTextureJsonConverter m_sceneTextureJsonConverter = {};
	};
}