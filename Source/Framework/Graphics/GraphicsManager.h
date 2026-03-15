#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init();
		bool Create(const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig);
		void PostCreateSetup(const HWND& a_hWND);

		void LoadConfig();
		void SaveConfig();

		void BeginDraw();
		void Draw();
		void EndDraw();

	private:

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		const std::filesystem::path k_configFileIOPath = "Asset/Data/Config/Graphics/GraphicsConfig.json";

		Hardware m_hardware = Hardware();
		
		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		GraphicsManager()           = default;
		~GraphicsManager() override = default;
	};
}