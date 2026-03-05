#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init           ();
		bool Create         (const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig);
		void PostCreateSetup(const HWND& a_hWND);

		void LoadConfig();
		void SaveConfig();

		void BeginFrame();
		void Draw      ();
		void EndFrame  ();

		const auto& GetHardware() const { return m_hardware; }
		
		auto& GetResourceContext() { return m_resourceContext; }

	private:

		const std::string k_configFileIOPath = "Asset/Data/Config/Graphics/GraphicsConfig.json";

		ShaderCompiler  m_shaderCompiler;
		Hardware        m_hardware;
		SwapChain       m_swapChain;
		Renderer        m_renderer;
		ResourceContext m_resourceContext;
		TextureContext  m_textureContext;

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		GraphicsManager ();
		~GraphicsManager() override;
	};
}