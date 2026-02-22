#pragma once

namespace FWK
{
	class Window final
	{
	public:

		Window ();
		~Window();

		void Init  ();
		bool Create(const std::string& a_titleName, const std::string& a_windowClassName);
		
		void LoadCONFIG();
		void SaveCONFIG();

		bool ProcessMessage() const;

		bool HasHWND() const;

		// 現在のモジュール(アプリ本体)のインスタンスハンドルを取得する
		// インスタンハンドルは実行中のアプリケーションモジュールを一意に識別する識別子
		HINSTANCE GetInstanceHandle() const { return GetModuleHandle(nullptr); }

		const auto& GetWindowConfig() const { return m_windowConfig; }

		const HWND& GetHWND() const { return m_hWND; }
		
		DWORD GetWindowStyle() const;

		void SetWindowConfig(const CommonStruct::WindowConfig& a_set) { m_windowConfig = a_set; }

		// ボーダーレス、フルウィンドウ、ノーマルウィンドウに切り替えることができる
		void ApplyWindowConfig(const CommonStruct::WindowConfig& a_windowConfig);

	private:

		static LRESULT CALLBACK CallWindowProcedure(const HWND   a_hWND,
												    const UINT   a_message,
												    const WPARAM a_wParam,
												    const LPARAM a_lParam);

		LRESULT CALLBACK WindowProcedure(const HWND   a_hWND,
										 const UINT   a_message,
										 const WPARAM a_wParam,
										 const LPARAM a_lParam);

		void Release();

		void SetClientSize();

		// ウィンドウのタイトルバー、最小化、最大化機能を持たせウィンドウのサイズ変更機能を除外したスタイル
		static constexpr DWORD k_defaultStyle = WS_OVERLAPPEDWINDOW - WS_THICKFRAME;

		static constexpr UINT k_windowStyle      = CS_HREDRAW | CS_VREDRAW;
		static constexpr UINT k_timeResolutionMS = 1U;
		static constexpr UINT k_msgFilterMIN     = 0U;
		static constexpr UINT k_msgFilterMAX     = 0U;

		static constexpr int k_classExtraBytes  = 0;
		static constexpr int k_windowExtraBytes = 0;

		static constexpr int k_defaultWindowPosX = 0;
		static constexpr int k_defaultWindowPosY = 0;

		const std::string k_configFileIOPath = "Asset/Data/CONFIG/Window/WindowCONFIG.json";

		Converter::WindowJsonConverter m_windowJsonConverter = Converter::WindowJsonConverter{ *this };
		
		CommonStruct::WindowConfig m_windowConfig = {};

		HWND m_hWND = nullptr;
	};
}