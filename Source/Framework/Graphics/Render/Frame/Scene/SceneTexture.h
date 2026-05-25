#pragma once

namespace FWK::Graphics
{
	class SceneTexture final
	{
	private:

		using RenderTargetTextureMap = std::unordered_map<TypeAlias::TypeTag, std::weak_ptr<RenderTargetTexture>>;
		using DepthStencilTextureMap = std::unordered_map<TypeAlias::TypeTag, std::weak_ptr<DepthStencilTexture>>;

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

		void AddRenderTargetTexture(const TypeAlias::TypeTag a_typeTag, const std::shared_ptr<RenderTargetTexture>& a_renderTargetTexture);
		void AddDepthStencilTexture(const TypeAlias::TypeTag a_typeTag, const std::shared_ptr<DepthStencilTexture>& a_depthStencilTexture);

		std::weak_ptr<RenderTargetTexture> FindVALRenderTargetTexture(const TypeAlias::TypeTag a_typeTag) const;
		std::weak_ptr<DepthStencilTexture> FindVALDepthStencilTexture(const TypeAlias::TypeTag a_typeTag) const;

		std::weak_ptr<RenderTargetTexture> GetVALFinalSceneTexture			  () const { return m_finalSceneTexture; }
		std::weak_ptr<DepthStencilTexture> GetVALFinalSceneDepthStencilTexture() const { return m_finalSceneDepthStencilTexture; }
		
		const auto& GetRenderTargetTextureList() const { return m_renderTargetTextureList; }
		const auto& GetDepthStencilTextureList() const { return m_depthStencilTextureList; }

	private:

		static constexpr DXGI_FORMAT k_defaultSceneColorFormat = Constant::k_defaultSwapChainBackBufferFormat;

		RenderTargetTextureMap m_renderTargetTextureMap = {};
		DepthStencilTextureMap m_depthStencilTextureMap = {};

		std::vector<std::shared_ptr<RenderTargetTexture>> m_renderTargetTextureList = {};
		std::vector<std::shared_ptr<DepthStencilTexture>> m_depthStencilTextureList = {};

		std::shared_ptr<RenderTargetTexture> m_finalSceneTexture		     = nullptr;
		std::shared_ptr<DepthStencilTexture> m_finalSceneDepthStencilTexture = nullptr;

		Converter::SceneTextureJsonConverter m_sceneTextureJsonConverter = {};
	};
}