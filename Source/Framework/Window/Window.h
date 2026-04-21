#pragma once

namespace FWK
{
	class Window final
	{
	public:

		 Window();
		~Window();

		void Init      ();
		void LoadCONFIG();
		bool Create    (const std::wstring& a_windowClassName, const std::string& a_titleName);

		bool ProcessMessages() const;

		void SaveCONFIG() const;

		bool HasHWND() const;

		void SetWindowConfig(const auto& a_set) { m_windowCONFIG = a_set; }

		const auto& GetREFHWND() const { return m_hwnd; }

		const auto& GetREFWindowCONFIG() const { return m_windowCONFIG; }

	private:

		// Win32APIに渡すウィンドウプロシージャは通常のメンバ関数では渡せないため、
		// static関数として定義して呼び出しの入口にする
		static LRESULT CALLBACK CallWindowProcedure(const HWND   a_hwnd,
												    const UINT   a_message,
												    const WPARAM a_wParam,
												    const LPARAM a_lParam);

		LRESULT CALLBACK WindowProcedure(const HWND   a_hwnd,
										 const UINT   a_message,
										 const WPARAM a_wParam,
										 const LPARAM a_lParam);

		bool CreateWindowInstance(const std::wstring& a_windowClassName, const std::string& a_titleName);

		void SetupClientSize();

		void Release();

		HINSTANCE FetchVALInstanceHandle() const;

		DWORD FetchVALWindowStyle() const;

		static constexpr DWORD k_generalWindowStyle = WS_OVERLAPPEDWINDOW - WS_THICKFRAME;

		static constexpr UINT k_windowStyle = CS_HREDRAW | CS_VREDRAW;

		static constexpr UINT k_timeResolutionMS = 1U;

		static constexpr UINT k_msgFilterMIN = 0U;
		static constexpr UINT k_msgFilterMAX = 0U;

		// ウィンドウのタイトルバー、最小化、最大化機能を持たせウィンドウのサイズ変更機能を除外したスタイル
		static constexpr std::wstring_view k_windowInstancePropertyName = L"GameWindowInstance";

		static constexpr int k_classExtraBytes  = 0;
		static constexpr int k_windowExtraBytes = 0;

		static constexpr int k_defaultWindowPositionX = 0;
		static constexpr int k_defaultWindowPositionY = 0;

		const std::filesystem::path k_configFileIOPath = "Asset/Data/CONFIG/Window/WindowCONFIG.json";

		HWND m_hwnd = nullptr;

		Converter::WindowJsonConverter m_windowJsonConverter = {};

		Struct::WindowCONFIG m_windowCONFIG = {};
	};
}