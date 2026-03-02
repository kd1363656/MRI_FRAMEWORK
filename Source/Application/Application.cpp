#include "Application.h"

// SALアノテーション付きWinMain関数(各引数の意味や使用条件をコンパイラや静的解析ツールに伝えるための注釈)
int WINAPI WinMain(_In_     HINSTANCE, 
				   _In_opt_ HINSTANCE,
				   _In_     LPSTR,
				   _In_     int)
{
	// デバッグ時にメモリリークを検出する設定
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | 
				   _CRTDBG_LEAK_CHECK_DF);

	// COMライブラリの初期化(WICやDXGI内部でも使用される)
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) { return -1; }

	Application::GetInstance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

void Application::Execute()
{
	Init        ();
	Load        ();
	PostLoadInit();

	while (true)
	{
		// FPSの計測
		m_fpsController.UpdateStartTime();

		// ウィンドウメッセージの処理
		m_window.ProcessMessage();

		// ウィンドウズハンドルがなくなるかエスケープキーを押されたらゲームループを抜ける
		if (GetAsyncKeyState(VK_ESCAPE) ||
			!m_window.HasHWND()) 
		{
			break; 
		}

		// フレームレート制御
		m_fpsController.Update();

		UpdateWindowTitleBar();
	}

	// もしゲームデータがセーブされていなくても変更が適応されるべき項目をセーブする関数
	Save();
}

void Application::Init()
{
	m_window.Init       ();
	m_fpsController.Init();

	// ウィンドウ作成後すぐにFPSを表示
	// (別にしなくてもいいがいきなりタイトルバーに変化があれば不快だと思ったので)
	UpdateWindowTitleBar();
}
void Application::PostLoadInit()
{
	if (!m_window.Create(k_titleBar, k_windowClassName))
	{
		assert(false && "失敗");
		return;
	}
}

void Application::Load()
{
	m_window.LoadConfig		  ();
	m_fpsController.LoadConfig();
}
void Application::Save()
{
	m_window.SaveConfig       ();
	m_fpsController.SaveConfig();
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + FPSの表示
	const std::string& l_titleBar = GenerateWindowTitleText();

	SetWindowTextA(GetHWND(), l_titleBar.c_str());
}

std::string Application::GenerateWindowTitleText() const
{
	return std::format("{} : {}", k_titleBar, static_cast<int>(GetNowFPS()));
}