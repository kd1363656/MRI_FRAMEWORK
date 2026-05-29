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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COMライブラリの初期化(WICやDXGI内部でも使用される)
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) { return Application::GetVALExitCodeCOMInitializeFailed(); }

	Application::GetInstance().Execute();

	// COM解放
	CoUninitialize();

	return Application::GetVALExitCodeSuccess();
}

void Application::Execute()
{
	auto& l_graphicsManager = FWK::Graphics::GraphicsManager::GetInstance();
	auto& l_sceneManager    = FWK::SceneManager::GetInstance		     ();
	auto& l_editorManager   = FWK::Editor::EditorManager::GetInstance	 ();

	// 初期化関係処理
	INIT      (l_graphicsManager);
	LoadCONFIG(l_graphicsManager, l_editorManager);

	if (!PostLoadCONFIG(l_graphicsManager, l_sceneManager, l_editorManager))
	{
		assert(false && "アプリケーションのPostLoadSetup関数処理が失敗しました。");
		return;
	}

	// 描画コマンドポインタの登録
	RegisterDrawCommand(l_sceneManager);

	while (true)
	{
		// 更新
		if (!BeginFrame(l_graphicsManager)) { break; }

		Update(l_sceneManager);

		// 描画
		BeginDraw(l_graphicsManager);
		Draw	 (l_graphicsManager);
		EndDraw  (l_graphicsManager);

		// FPSの更新
		EndFrame(l_graphicsManager);
	}

	// もしゲームデータがセーブされていなくても変更が適用されるべき項目を自動セーブする
	SaveCONFIG(l_graphicsManager, l_editorManager);
}

void Application::INIT(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	m_window.INIT		  ();
	a_graphicsManager.INIT();
}
void Application::LoadCONFIG(FWK::Graphics::GraphicsManager& a_graphicsManager, FWK::Editor::EditorManager& a_editorManager)
{
	m_window.LoadCONFIG			();
	m_fpsController.LoadCONFIG  ();
	a_graphicsManager.LoadCONFIG();
	a_editorManager.LoadCONFIG  ();
}
bool Application::PostLoadCONFIG(FWK::Graphics::GraphicsManager& a_graphicsManager, FWK::SceneManager& a_sceneManager, FWK::Editor::EditorManager& a_editorManager)
{
	if (!m_window.Create(k_windowClassName, k_titleName))
	{
		assert(false && "ウィンドウの作成処理に失敗しました。");
		return false;
	}

	// ウィンドウサイズが決定してからウィンドウサイズに依存する
	// デプスステンシルテクスチャやレンダーターゲットテクスチャのサイズを決める
	a_graphicsManager.PostDeserializeSetup(m_window.GetREFWindowCONFIG());

	if (!a_graphicsManager.Create(m_window.GetREFHWND(), m_window.GetREFWindowCONFIG()))
	{
		assert(false && "グラフィックスの作成処理に失敗しました。");
		return false;
	}

	// Create処理が終わった後に実行する処理
	a_graphicsManager.PostCreateSetup(m_window.GetREFHWND());
	a_sceneManager.PostLoadSetup     ();

	// GraphicsManagerでSRVを作り終わった後にIMGUIを初期化
	a_editorManager.INIT(m_window.GetREFHWND());

	return true;
}

void Application::RegisterDrawCommand(const FWK::SceneManager& a_sceneManager) const
{
	a_sceneManager.RegisterDrawCommand();
}

bool Application::BeginFrame(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	// FPSの計測開始
	m_fpsController.BeginFrame();

	// ウィンドウメッセージの処理
	if (!m_window.ProcessMessages()) { return false; }

	// ウィンドウズハンドルを所持していないかエスケープキーを押されたらreturn
	if ( GetAsyncKeyState(VK_ESCAPE) ||
		!m_window.HasHWND())
	{
		return false;
	}

	// 描画するためのテクスチャなどを動的にロードする処理
	a_graphicsManager.BeginFrame();

	return true;
}

void Application::BeginDraw(FWK::Graphics::GraphicsManager& a_graphicsManager) const
{
	a_graphicsManager.BeginDraw();
}
void Application::Draw(FWK::Graphics::GraphicsManager& a_graphicsManager) const
{
	a_graphicsManager.Draw();
}
void Application::EndDraw(FWK::Graphics::GraphicsManager& a_graphicsManager) const
{
	FWK::Editor::EditorManager::GetInstance().DrawEditor();

	a_graphicsManager.EndDraw();
}

void Application::Update(FWK::SceneManager& a_sceneManager) const
{
	a_sceneManager.Update();
}

void Application::EndFrame(FWK::Graphics::GraphicsManager& a_graphicsManager)
{
	a_graphicsManager.EndFrame();

	// フレームレート制御
	m_fpsController.EndFrame();

	UpdateWindowTitleBar();
}

void Application::SaveCONFIG(const FWK::Graphics::GraphicsManager& a_graphicsManager, const FWK::Editor::EditorManager& a_edtorManager) const
{
	m_window.SaveCONFIG         ();
	m_fpsController.SaveCONFIG  ();
	a_graphicsManager.SaveCONFIG();
	a_edtorManager.SaveCONFIG   ();
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + FPSのテキスト
	const auto& l_text = std::format("{} : {}", k_titleName, static_cast<int>(m_fpsController.GetVALCurrentFPS()));

	// ウィンドウバーに表示
	SetWindowTextA(m_window.GetREFHWND(), l_text.c_str());
}