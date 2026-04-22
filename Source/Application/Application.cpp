#include "Application.h"

// SALアノテーション付きWinMain関数(各引数の意味や使用条件をコンパイラや静的解析ツールに伝えるための注釈)
// int WINAPI WinMain(この実行ファイル自体を表すインスタンスハンドル、
//					  昔のWindowsとの互換性のために渡される値、
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
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) { return Application::GetVALExitCodeCOMInitFailed(); }

	Application::GetInstance().Execute();

	// COM解放
	CoUninitialize();

	return Application::GetVALExitCodeSuccess();
}

void Application::Execute()
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();

	// 初期化関係処理
	Init    (l_graphicsManager);
	LoadFile(l_graphicsManager);

	if (!PostLoadSetup(l_graphicsManager))
	{
		assert(false && "アプリケーションのPostLoadSetup関数処理が失敗しました。");
		return;
	}

	while (true)
	{
		// 更新
		if (!BeginFrame()) { break; }

		// 描画
		BeginDraw(l_graphicsManager);
		EndDraw  (l_graphicsManager);

		// FPSの更新
		EndFrame(l_graphicsManager);
	}

	// もしゲームデータがセーブされていなくても変更が適用されるべき項目をセーブする
	SaveFile(l_graphicsManager);
}

void Application::Init(const FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	m_window.Init();
	
	a_graphicsManager.Init();
}

void Application::LoadFile(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	m_window.LoadCONFIG       ();
	m_fpsController.LoadCONFIG();

	a_graphicsManager.LoadCONFIG();
}

bool Application::PostLoadSetup(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	if (!m_window.Create(k_windowClassName, k_titleName))
	{
		assert(false && "ウィンドウの作成処理に失敗しました。");
		return false;
	}

	if (!a_graphicsManager.Create(m_window.GetREFHWND(), m_window.GetREFWindowCONFIG()))
	{
		assert(false && "グラフィックスの作成処理に失敗しました。");
		return false;
	}

	// Create処理が終わった後に実行する処理
	a_graphicsManager.PostCreateSetup(m_window.GetREFHWND());

	return true;
}

bool Application::BeginFrame()
{
	// FPSの計測開始
	m_fpsController.BeginFrame();

	// ウィンドウメッセージの処理
	if (!m_window.ProcessMessages()) { return false; }

	// ウィンドウズハンドルを所持していないかエスケープキーを押されたらreturn
	if (GetAsyncKeyState(VK_ESCAPE) ||
		!m_window.HasHWND())
	{
		return false;
	}

	return true;
}

void Application::BeginDraw(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	a_graphicsManager.BeginDraw();
}
void Application::EndDraw(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	a_graphicsManager.EndDraw();
}

void Application::EndFrame(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	a_graphicsManager.EndFrame();

	// フレームレート制御
	m_fpsController.EndFrame();

	UpdateWindowTitleBar();
}

void Application::SaveFile(FWK::Graphics::GraphicsManager& a_graphicsManager) const
{
	m_window.SaveCONFIG		  ();
	m_fpsController.SaveCONFIG();

	a_graphicsManager.SaveCONFIG();
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + FPSのテキスト
	const auto& l_text = std::format("{} : {}", k_titleName, static_cast<int>(m_fpsController.GetVALCurrentFPS()));

	// ウィンドウバーに表示
	SetWindowTextA(m_window.GetREFHWND(), l_text.c_str());
}