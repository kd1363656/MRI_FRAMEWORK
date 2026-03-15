#include "Application.h"

// SALアノテーション付きWinMain関数(各引数の意味や使用条件をコンパイラや静的解析ツールに伝えるための注釈)
// int WINAPI WinMain(この実行ファイル自体を表すインスタンスハンドル、
//					  昔のWindowsとのご寛容で割らされる値、
//					  コマンドライン引数、
//					  ウィンドウを最初にどう表示するかを表す値)

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
	Init();

	Load();

	if (!Create())
	{
		assert(false && "アプリケーションのCreate関数の呼び出しに失敗しました。");
		return;
	}

	PostCreateSetup();

	while (true)
	{
		// 更新
		// falseが帰ったらゲームループを抜ける
		if (!BeginUpdate()) { break; }

		EndUpdate();
	}

	// もしゲームデータがセーブされていなくても変更が適応されるべき項目をセーブする関数
	Save();
}

void Application::Init()
{
	m_window.Init();

	m_fpsController.Init();

	FWK::Graphics::GraphicsManager::GetInstance().Init();
}
bool Application::Create()
{
	if (!m_window.Create(k_titleName, k_windowClassName))
	{
		assert(false && "ウィンドウの作成処理に失敗しました。");
		return false;
	}

	if (!FWK::Graphics::GraphicsManager::GetInstance().Create(FetchHWND(), m_window.GetWindowConfig()))
	{
		assert(false && "グラフィックスの作成処理に失敗しました。");
		return false;
	}

	return true;
}

void Application::PostCreateSetup() const
{
}

void Application::Load()
{
	m_window.LoadConfig();

	m_fpsController.LoadConfig();

	FWK::Graphics::GraphicsManager::GetInstance().LoadConfig();
}
void Application::Save()
{
	m_window.SaveConfig();

	m_fpsController.SaveConfig();

	FWK::Graphics::GraphicsManager::GetInstance().SaveConfig();
}

bool Application::BeginUpdate()
{
	// FPSの計測開始
	m_fpsController.BeginUpdate();

	// ウィンドウメッセージの処理
	m_window.ProcessMessages();

	// ウィンドウズハンドルがなくなるかエスケープキーを押されたらreturn
	if (GetAsyncKeyState(VK_ESCAPE) ||
		!m_window.HasHWND())
	{
		return false;
	}

	return true;
}
void Application::EndUpdate()
{
	// フレームレート制御
	m_fpsController.EndUpdate();

	UpdateWindowTitleBar();
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + FPSの表示
	const std::string& l_titleText = GenerateWindowTitleText();

	SetWindowTextA(FetchHWND(), l_titleText.c_str());
}

std::string Application::GenerateWindowTitleText() const
{
	return std::format("{} : {}", k_titleName, static_cast<int>(FetchCurrentFPS()));
}

const HWND& Application::FetchHWND() const
{
	return m_window.GetHWND();
}

float Application::FetchCurrentFPS() const
{
	return m_fpsController.GetCurrentFPS();
}