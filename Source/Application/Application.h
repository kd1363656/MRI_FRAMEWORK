#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:
	
	void Execute();

private:

	void Init();
	bool Create();
	void PostCreateSetup() const;
	
	void Load();
	void Save();

	bool BeginUpdate();
	void EndUpdate();

	void UpdateWindowTitleBar() const;

	std::string GenerateWindowTitleText() const;

	const HWND& FetchHWND() const;

	float FetchCurrentFPS() const;

	const std::string  k_titleName       = "MRI_FRAMEWORK";
	const std::wstring k_windowClassName = L"Window";

	FWK::Window m_window = FWK::Window();

	FWK::FPSController m_fpsController = FWK::FPSController();

	//=========================
	// シングルトン
	//=========================
	friend class SingletonBase<Application>;

	Application()           = default;
	~Application() override = default;
};