#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init           () const;
		void LoadCONFIG     ();
		bool Create         (const HWND& a_hwnd, const Struct::WindowCONFIG& a_windowCONFIG);
		void PostCreateSetup(const HWND& a_hwnd);

		void BeginDraw();
		void Draw	  () const;
		void EndDraw  ();
		void EndFrame ();

		void SaveCONFIG() const;

		const auto& GetREFResourceContext() const { return m_resourceContext; }
		const auto& GetREFRenderer       () const { return m_renderer; }
		const auto& GetREFSwapChain      () const { return m_swapChain; }

		auto& GetMutableREFResourceContext() { return m_resourceContext; }
		auto& GetMutableREFRenderer       () { return m_renderer; }
		auto& GetMutableREFSwapChain	  () { return m_swapChain; }

	private:

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		const std::filesystem::path k_configFileIOPath = "Asset/Data/CONFIG/Graphics/GraphicsCONFIG.json";

		// 本来ならm_renderer,m_swapChainの順が依存関係的に正しいが
		// デストラクタの呼び出し順の都合上並びを変えている
		Factory         m_factory         = {};
		Device          m_device          = {};
		ResourceContext m_resourceContext = {};
		SwapChain       m_swapChain       = {};
		ShaderCompiler  m_shaderCompiler  = {};
		Renderer        m_renderer        = {};

		JsonConverter::GraphicsManagerJsonConverter m_graphicsManagerJsonConverter = {};
		
		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		 GraphicsManager()          = default;
		~GraphicsManager() override = default;
	};
}