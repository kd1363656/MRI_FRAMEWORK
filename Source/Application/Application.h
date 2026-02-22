#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute();

	const HWND& GetHWND() const { return m_window.GetHWND(); }
	
	float GetNowFPS() const { return m_fpsController.GetNowFPS(); }

private:

	void Init        ();
	void PostLoadInit();

	void Load();
	void Save();

	void UpdateWindowTitleBar() const;

	std::string GenerateWindowTitleText() const;

	const std::string k_titleBar        = "MRI_FRAMEWORK";
	const std::string k_windowClassName = "Window";

	FWK::Window m_window = {};
	
	FWK::FPSController m_fpsController = {};

	//=========================
	// ƒVƒ“ƒOƒ‹ƒgƒ“
	//=========================
	friend class SingletonBase<Application>;

	Application ()          = default;
	~Application() override = default;
};