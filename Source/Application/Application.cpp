#include "Application.h"

int WINAPI WinMain(_In_     HINSTANCE, 
				   _In_opt_ HINSTANCE,
				   _In_     LPSTR,
				   _In_     int)
{
	// メモリリークを知らせる。
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	// ウィンドウダイアログなどを使う際に必要
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) { return -1; }

	Application::GetInstance().Execute();

	// "COM"解放
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

void Application::Init()
{
	// "TypeInfo"の登録
	FWK::Utility::TypeInfo::RegisterAll();

	m_window.Init       ();
	m_fpsController.Init();

	// 描画クラスの初期化
	FWK::Render::Renderer::GetInstance().Init();

	// ウィンドウ作成後すぐにFPSを表示
	// (別にしなくてもいいがいきなりタイトルバーに変化があれば不快だと思ったので)
	UpdateWindowTitleBar();
}

void Application::PostLoadInit()
{
	if (!m_window.Create(k_titleBar, k_windowClassName))
	{
		assert(false && "ウィンドウ作成に失敗。");
		return;
	}

	auto& l_renderer = FWK::Render::Renderer::GetInstance();

	if (!l_renderer.Create(GetHWND(), m_window.GetWindowConfig()))
	{
		assert(false && "レンダラーの必要クラス作成に失敗。");
		return;
	}

	// Create処理が終わった後のレンダラーの初期化
	l_renderer.PostCreateInit(GetHWND());
}

void Application::Load()
{
	m_window.LoadCONFIG							   ();
	FWK::Render::Renderer::GetInstance().LoadCONFIG();
	m_fpsController.LoadCONFIG					   ();
}
void Application::Save()
{
	m_window.SaveCONFIG							   ();
	FWK::Render::Renderer::GetInstance().SaveCONFIG();
	m_fpsController.SaveCONFIG					   ();
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