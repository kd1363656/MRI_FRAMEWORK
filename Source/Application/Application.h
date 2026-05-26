#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute();
	
	static constexpr int GetVALExitCodeSuccess			  () { return k_exitCodeSuccess; }
	static constexpr int GetVALExitCodeCOMInitializeFailed() { return k_exitCodeCOMInitializeFailed; }

private:

	void INIT          (FWK::Graphics::GraphicsManager& a_graphicsManager);
	void LoadCONFIG    (FWK::Graphics::GraphicsManager& a_graphicsManager);
	bool PostLoadCONFIG(FWK::Graphics::GraphicsManager& a_graphicsManager, FWK::SceneManager& a_sceneManager, FWK::Editor::EditorManager& a_editorManager);

	bool BeginFrame(FWK::Graphics::GraphicsManager& a_graphicsManager);
	
	void RequestDraw(const FWK::SceneManager&			   a_sceneManager)    const;
	void BeginDraw  (      FWK::Graphics::GraphicsManager& a_graphicsManager) const;
	void Draw       (	   FWK::Graphics::GraphicsManager& a_graphicsManager) const;
	void EndDraw    (      FWK::Graphics::GraphicsManager& a_graphicsManager) const;

	void Update(FWK::SceneManager& a_sceneManager) const;

	void EndFrame(FWK::Graphics::GraphicsManager& a_graphicsManager);

	void SaveCONFIG(const FWK::Graphics::GraphicsManager& a_graphicsManager) const;

	void UpdateWindowTitleBar() const;

	static constexpr int k_exitCodeSuccess			   =  0;
	static constexpr int k_exitCodeCOMInitializeFailed = -1;

	const std::wstring k_windowClassName = L"Window";
	const std::string  k_titleName       = "MRI_FRAMEWORK";

	FWK::Window        m_window        = {};
	FWK::FPSController m_fpsController = {};

	//=========================
	// シングルトン
	//=========================
	friend class SingletonBase<Application>;

	 Application()          = default;
	~Application() override = default;
};