#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute();
	
	static constexpr int GetVALExitCodeSuccess		() { return k_exitCodeSuccess; }
	static constexpr int GetVALExitCodeCOMInitFailed() { return k_exitCodeCOMInitFailed; }

private:

	void Init         (const FWK::Graphics::GraphicsManager& a_graphicsManager);
	void LoadFile     (      FWK::Graphics::GraphicsManager& a_graphicsManager);
	bool PostLoadSetup(      FWK::Graphics::GraphicsManager& a_graphicsManager);

	bool BeginFrame();

	void BeginDraw(FWK::Graphics::GraphicsManager& a_graphicsManager);
	void EndDraw  (FWK::Graphics::GraphicsManager& a_graphicsManager);

	void EndFrame(FWK::Graphics::GraphicsManager& a_graphicsManager);

	void SaveFile(FWK::Graphics::GraphicsManager& a_graphicsManager) const;

	void UpdateWindowTitleBar() const;

	static constexpr int k_exitCodeSuccess       =  0;
	static constexpr int k_exitCodeCOMInitFailed = -1;

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