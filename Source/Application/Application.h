#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute();
	
private:

	void Init         ();
	void LoadFile     ();
	bool PostLoadSetup();

	bool BeginFrame();

	void EndFrame();

	void SaveFile() const;

	std::string GenerateWindowTitleText() const;
	void		UpdateWindowTitleBar   () const;

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