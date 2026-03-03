#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init         ();
		bool Create       (const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig);
		void PostLoadSetup(const HWND& a_hWND);

		void LoadConfig();
		void SaveConfig();

		void BeginFrame();
		void Draw      ();
		void EndFrame  ();

	private:

		const std::string k_configFileIOPath = "Asset/Data/Config/Graphics/GraphicsConfig.json";

		Hardware m_hardware;
		Renderer m_renderer;

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		GraphicsManager ();
		~GraphicsManager() override;
	};
}