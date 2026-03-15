#pragma once

namespace FWK
{
	class Window final
	{
	public:

		Window();
		~Window();

		void Init();
		bool Create(const std::string& a_titleName, const std::wstring& a_windowClassName);

		void LoadConfig();
		void SaveConfig();

		bool ProcessMessages() const;

		bool HasHWND() const;

		void SetWindowConfig(const CommonStruct::WindowConfig& a_set) { m_windowConfig = a_set; }

		const auto& GetWindowConfig() const { return m_windowConfig; }
		
		const HWND& GetHWND() const { return m_hwnd; }

	private:
		
		bool CreateWindowInstance(const std::string& a_titleName, const std::wstring& a_windowClassName);

		void Release();

		static LRESULT CALLBACK CallWindowProcedure(const HWND   a_hWND,
												    const UINT   a_message,
												    const WPARAM a_wParam,
												    const LPARAM a_lParam);

		LRESULT CALLBACK WindowProcedure(const HWND   a_hWND,
										 const UINT   a_message,
										 const WPARAM a_wParam,
										 const LPARAM a_lParam);

		void SetupClientSize();

		HINSTANCE FetchInstanceHandle() const;

		DWORD FetchWindowStyle() const;

		static constexpr DWORD k_generalWindowStyle = WS_OVERLAPPEDWINDOW - WS_THICKFRAME;

		static constexpr UINT k_windowStyle = CS_HREDRAW | CS_VREDRAW;

		static constexpr UINT k_timeResolutionMS = 1U;

		static constexpr UINT k_msgFilterMIN = 0U;
		static constexpr UINT k_msgFilterMAX = 0U;

		// ウィンドウのタイトルバー、最小化、最大化機能を持たせウィンドウのサイズ変更機能を除外したスタイル
		static constexpr std::wstring_view k_windowInstancePropertyName = L"GameWindowInstance";

		static constexpr int k_classExtraBytes  = 0;
		static constexpr int k_windowExtraBytes = 0;

		static constexpr int k_defaultWindowPosX = 0;
		static constexpr int k_defaultWindowPosY = 0;

		const std::filesystem::path k_configFileIOPath = "Asset/Data/Config/Window/WindowConfig.json";

		HWND m_hwnd;

		Converter::WindowJsonConverter m_windowJsonConverter;

		CommonStruct::WindowConfig m_windowConfig;
	};
}