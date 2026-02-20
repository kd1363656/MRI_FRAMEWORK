#include "Application.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	// メモリリークを知らせる。
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// "COM"初期化
	// ウィンドウダイアログなどを使う際に必要
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		return -1;
	}

	Application::GetInstance().Execute();

	// "COM"解放
	CoUninitialize();

	return 0;
}

void Application::Execute()
{
	Init();
	Load();

	if (!PostLoad())
	{
		assert(false && "\"PostLoad\"に失敗しました、アプリケーションの実行を中断します。");
		return;
	}

	while (true)
	{
		// "FPS"の計測
		m_fpsController.UpdateStartTime();

		// ウィンドウメッセージの処理
		m_window.ProcessMessage();

		if (GetAsyncKeyState(VK_ESCAPE) || !m_window.HasHWND())
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
bool Application::PostLoad()
{
	if (!m_window.Create(k_titleBar, k_windowClassName))
	{
		assert(false && "ウィンドウ作成に失敗。");
		return false;
	}

	return true;
}

void Application::Init()
{
	// "TypeInfo"の登録
	FWK::Utility::TypeInfo::RegisterAll();

	m_window.Init       ();
	m_fpsController.Init();
	
	// ウィンドウ作成後すぐに"FPS"を表示
	// (別にしなくてもいいがいきなりタイトルバーに変化があれば不快だと思ったので)
	UpdateWindowTitleBar();
}

void Application::Load()
{
	m_window.LoadConfig       ();
	m_fpsController.LoadConfig();
}
void Application::Save()
{
	m_window.SaveConfig       ();
	m_fpsController.SaveConfig();
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + "FPS"の表示
	const std::string& l_titleBar = GenerateWindowTitleText();

	SetWindowTextA(GetHWND(), l_titleBar.c_str());
}

std::string Application::GenerateWindowTitleText() const
{
	return std::string();	return std::format("{} : {}", k_titleBar, static_cast<int>(GetNowFPS()));
}