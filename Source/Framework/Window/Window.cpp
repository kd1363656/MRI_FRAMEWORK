#include "Window.h"

FWK::Window::Window () = default;
FWK::Window::~Window()
{
	Release();
}

void FWK::Window::Init()
{
	// ウィンドウのデフォルト値を格納しておく(jsonのロードが失敗してもプログラムが動くように)
	m_windowConfig.width    = CommonConstant::k_defaultWindowWidth;
	m_windowConfig.height   = CommonConstant::k_defaultWindowHeight;
	m_windowConfig.styleTag = FWK::GetTypeInfo<FWK::Tag::WindowStyleNormalTag>().k_id;
}
bool FWK::Window::Create(const std::string& a_titleName, const std::string& a_windowClassName)
{
	// DPI対応(Windows10以降)
	SetProcessDPIAware();

	// ハンドル取得
	HINSTANCE l_hInstance = GetInstanceHandle();

	// マルチバイト文字列に変換
	const auto& l_windowClassName = Utility::String::StringToWString(a_windowClassName);
	const auto& l_titleName		  = Utility::String::StringToWString((a_titleName));

	// ウィンドウクラス登録パラメータの設定
	auto l_wc = WNDCLASSEX();

	l_wc.cbSize        = sizeof(WNDCLASSEX);				                   // 構造体のサイズを設定
	l_wc.style         = k_windowStyle;					                   // ウィンドウの動作スタイル(サイズ変更時に再描画など)
	l_wc.lpfnWndProc   = &CallWindowProcedure;				               // 指定した関数がウィンドウに届いたすべてのメッセージを処理
	l_wc.cbClsExtra    = k_classExtraBytes;					               // ウィンドウクラス用に追加で確保するメモリサイズ
	l_wc.cbWndExtra    = k_windowExtraBytes;					               // ウィンドウごとに追加で確保するメモリサイズ
	l_wc.hInstance     = l_hInstance;						               // アプリケーションのインスタンスハンドル
	l_wc.hIcon         = LoadIcon              (nullptr, IDI_APPLICATION); // ウィンドウに表示するアイコン
	l_wc.hIconSm       = LoadIcon              (nullptr, IDI_APPLICATION); // タスクバーやタイトルバーに表示されるアイコン
	l_wc.hCursor       = LoadCursor            (nullptr, IDC_ARROW);       // ウィンドウ上で使うマウスカーソルを指定
	l_wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);              // ウィンドウの背景色を指定
	l_wc.lpszMenuName  = nullptr;							               // メニューリソースの名前
	l_wc.lpszClassName = l_windowClassName.c_str();			               // ウィンドウクラス名

	// ウィンドウクラスをOSに登録
	if (!RegisterClassEx(&l_wc)) { return false; }

	// 現在のスタイル用のタグからスタイルパラメータを取得
	const DWORD l_styleTag = GetWindowStyle();

	// ウィンドウを生成しウィンドウズハンドルを取得
	m_hWND = CreateWindow(l_windowClassName.c_str(), //	登録済みウィンドウクラス名、
						  l_titleName.c_str(),		 // ウィンドウのタイトルバーの文字列、
						  l_styleTag,				 // ウィンドウのタイトル、
						  k_defaultWindowPosX,		 // ウィンドウ座標X、
						  k_defaultWindowPosY,		 // ウィンドウ座標Y、
						  m_windowConfig.width,		 // クライアント領域に幅、
						  m_windowConfig.height,	 // クライアント領域の高さ、
						  nullptr,					 // 親ウィンドウがない、
						  nullptr,					 // メニューなし、
						  l_hInstance,				 // アプリケーションのインスタンスハンドル、
						  this);					 // 作製するウィンドウにthisポインタを渡す,WM_CREATEなどで取り出せる	

	// 作製しが失敗したらreturn
	if (!m_hWND)
	{
		// メモリリーク防止
		UnregisterClass(l_windowClassName.c_str(), l_hInstance);
		return false;
	}

	// クライアントのサイズを設定
	SetClientSize();

	// ウィンドウの表示
	ShowWindow(m_hWND, SW_SHOW);

	// ウィンドウのクライアント領域をすぐに再描画
	UpdateWindow(m_hWND);

	// timeGetTime関数の制度を1msに設定(Sleep関数などに影響する)
	timeBeginPeriod(k_timeResolutionMS);

	return true;
}

void FWK::Window::LoadCONFIG()
{
	const auto& l_rootJson = Utility::FileIO::LoadJsonFile(k_configFileIOPath);

	// ウィンドウの幅、高さ、ボーダーレスウィンドウかなどの値をDeserialize
	m_windowJsonConverter.Deserialize(l_rootJson);
}
void FWK::Window::SaveCONFIG()
{
	const auto& l_rootJson = m_windowJsonConverter.Serialize();

	// ウィンドウの幅、高さ、ボーダーレスウィンドウかなどの値をSerialize
	Utility::FileIO::SaveJsonFile(l_rootJson, k_configFileIOPath);
}

bool FWK::Window::ProcessMessage() const
{
	// メッセージ取得
	MSG msg = {};

	while(PeekMessage(&msg,            // メッセージ情報を格納するMSG構造体のアドレス
					   nullptr,        // 全ウィンドウのメッセージを対象とする 
					   k_msgFilterMIN, // メッセージフィルターの下限値
					   k_msgFilterMAX, // メッセージフィルターの上限値
					   PM_REMOVE))     // 取得したメッセージをキューから削除するかどうか
	{
		// 終了メッセージが来たら
		if (msg.message == WM_QUIT) { return false; }

		// 押されたキーをもとに文字メッセージに変換(Aキー -> Aが押された)
		TranslateMessage(&msg);

		// GetMessageやPeekMessageで取得したメッセージを対応するウィンドウプロシージャに転送
		DispatchMessage(&msg);
	}

	return true;
}

bool FWK::Window::HasHWND() const
{
	if (!m_hWND)
	{
		return false;
	}

	return true;
}

DWORD FWK::Window::GetWindowStyle() const
{
	// 持っているタグからウィンドウスタイルを返す
	if (m_windowConfig.styleTag != GetTypeInfo<Tag::WindowStyleNormalTag>().k_id)
	{
		return WS_POPUP;
	}
	
	return k_defaultStyle;
}

void FWK::Window::ApplyWindowConfig(const CommonStruct::WindowConfig& a_windowConfig)
{
	if (!m_hWND) { return; }

	// ウィンドウコンフィグをセット
	m_windowConfig = a_windowConfig;

	RECT l_screenRect = {};

	// GetDesktopWindowを使ってデスクトップ全体を
	// 表すウィンドウハンドルを取得し画面サイズを取得
	GetWindowRect(GetDesktopWindow(), &l_screenRect);

	const DWORD l_style = GetWindowStyle();

	// ウィンドウスタイルを変更
	SetWindowLong(m_hWND, GWL_STYLE, l_style);

	// ウィンドウの位置、サイズ、Zオーダーを制御する関数
	// SetWindowPos
	// (
	//		ウィンドウハンドル、
	//		Z順に配置されたウィンドウの前にあるウィンドウへのハンドル、
	//		クライアント座標でのウィンドウの左側の新しい位置、
	//		クライアント座標でのウィンドウの上部の新しい位置、
	//		ウィンドウの新しい幅、
	//		ウィンドウの新しい高さ、
	//		ウィンドウのサイズ設定と配置フラグ
	// );

	// ウィンドウスタイルがフルスクリーンかそうじゃないかで切り替える
	if (m_windowConfig.styleTag == GetTypeInfo<Tag::WindowStyleNormalTag>().k_id)
	{
		SetWindowPos(m_hWND,
					 HWND_TOP,
					 k_defaultWindowPosX,
					 k_defaultWindowPosY,
					 CommonConstant::k_defaultWindowWidth, 
					 CommonConstant::k_defaultWindowHeight,
					 SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
	}
	else
	{
		SetWindowPos(m_hWND,
		             HWND_TOP,
					 k_defaultWindowPosX,
					 k_defaultWindowPosY,
					 l_screenRect.right,
					 l_screenRect.bottom,
					 SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
	}

	// クライアント領域を再設定
	SetClientSize();
}

LRESULT FWK::Window::CallWindowProcedure(const HWND   a_hWND,
										 const UINT   a_message,
									     const WPARAM a_wParam,
									     const LPARAM a_lParam)
{
	auto* l_this = static_cast<FWK::Window*>(GetProp(a_hWND, L"GameWindowInstance"));

	// nullptrの場合はデフォルト処理を実行
	if (!l_this)
	{
		if (a_message == WM_CREATE)
		{
			// CreateWindowで渡したパラメータを取得
			auto* l_createStruct = (CREATESTRUCT*)a_lParam;
			auto* l_window       = (FWK::Window*)l_createStruct->lpCreateParams;
		
			// ウィンドウプロパティにこのクラスのインスタンスアドレスを埋め込んでおく
			// 次回から、l_this->WindowProcedureの方へ処理が流れていく
			SetProp(a_hWND, L"GameWindowInstance", l_window);

			return 0U;
		}
		else
		{
			return DefWindowProc(a_hWND,
								 a_message,
								 a_wParam,
								 a_lParam);
		}
	}

	// インスタンス側のWindow関数を実行する
	return l_this->WindowProcedure(a_hWND,
								   a_message,
								   a_wParam,
								   a_lParam);
}

LRESULT CALLBACK FWK::Window::WindowProcedure(const HWND   a_hWND,
											  const UINT   a_message, 
											  const WPARAM a_wParam,
											  const LPARAM a_lParam)
{
	// メッセージによって処理を選択
	switch (a_message)
	{
		// ウィンドウのXボタンが押された
		case WM_CLOSE:
		{
			Release();
		}
		break;

		// ウィンドウ破棄直前
		case WM_DESTROY:
		{
			RemoveProp(a_hWND, L"GameWindowInstance");
			PostQuitMessage(0);
		}
		break;

		default:
		{
			// メッセージのデフォルト処理
			return DefWindowProc(a_hWND,
								 a_message,
								 a_wParam,
								 a_lParam);
		}
		break;
	}

	return 0;
}

void FWK::Window::Release()
{
	// もしウィンドウハンドルがインスタンス化されていたら解放
	if (!m_hWND) { return; }

	// 指定したウィンドウを破棄
	DestroyWindow(m_hWND);

	// ヌルポインタにすることでゲームループを抜けるための起点にする
	m_hWND = nullptr;
}

void FWK::Window::SetClientSize()
{
	if (!m_hWND) { return; }

	// ウィンドウの位置とサイズを同時に変更する関数
	// MoveWindow
	// (
	//		対象となるウィンドウのハンドル、
	//		ウィンドウ左上のX座標、
	//		ウィンドウ左上のY座標、
	//		ウィンドウの幅、
	//		ウィンドウの高さ、
	//		再描画を行うかどうか、
	// );

	if (m_windowConfig.styleTag == GetTypeInfo<Tag::WindowStyleFullScreenTag>().k_id)
	{
		m_windowConfig.width  = static_cast<std::uint32_t>(GetSystemMetrics(SM_CXSCREEN));
		m_windowConfig.height = static_cast<std::uint32_t>(GetSystemMetrics(SM_CYSCREEN));

		// ボーダーレスウィンドウ("WS_POPUP")はフレームがないので補正不要
		MoveWindow(m_hWND,
				   k_defaultWindowPosX,
				   k_defaultWindowPosY,
				   static_cast<int>(m_windowConfig.width),
				   static_cast<int>(m_windowConfig.height),
				   TRUE);
	}
	else 
	{
		// ウィンドウ幅を通常に戻す
		m_windowConfig.width  = CommonConstant::k_defaultWindowWidth;
		m_windowConfig.height = CommonConstant::k_defaultWindowHeight;

		// 通常ウィンドウ、クライアントサイズを基準にフレーム分補正
		RECT l_rcWND    = {};
		RECT l_rcClient = {};

		// ウィンドウ全体のサイズ、クライアント領域のサイズを取得
		GetWindowRect(m_hWND, &l_rcWND);
		GetClientRect(m_hWND, &l_rcClient);

		const int l_frameWidth  = (l_rcWND.right  - l_rcWND.left) - (l_rcClient.right  - l_rcClient.left); // 枠の幅
		const int l_frameHeight = (l_rcWND.bottom - l_rcWND.top)  - (l_rcClient.bottom - l_rcClient.top);  // 枠の高さ

		MoveWindow(m_hWND,									                // 対象のウィンドウ
				   l_rcWND.left,								                // 左上のX座標
				   l_rcWND.top,								                // 左上のY座標
				   static_cast<int>(m_windowConfig.width)  + l_frameWidth,  // ウィンドウ全体の幅(枠含む)
				   static_cast<int>(m_windowConfig.height) + l_frameHeight, // ウィンドウ残体の高さ(枠含む)
				   TRUE);													// ウィンドウを再描画するかどうか
	}
}