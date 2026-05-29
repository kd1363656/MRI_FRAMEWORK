#pragma once

namespace FWK
{
	class Window final
	{
	public:

		 Window();
		~Window();

		void INIT      ();
		void LoadCONFIG();
		bool Create    (const std::wstring& a_windowClassName, const std::string& a_titleName);

		bool ProcessMessages() const;

		void SaveCONFIG() const;

		bool HasHWND() const;

		bool RequestClientSize(const Struct::ClientSize& a_clientSize);

		Struct::WindowResizeRequest ConsumeResizeRequest();

		bool IsMinimized() const;

		void SetWindowCONFIG(const auto& a_set) { m_windowCONFIG = a_set; }

		Struct::ClientSize FetchVALClientSize() const;

		const Struct::ClientSize& GetREFClientSize() const { return m_windowCONFIG.m_clientSize; }

		const auto& GetREFHWND() const { return m_hwnd; }

		const auto& GetREFWindowCONFIG() const { return m_windowCONFIG; }

	private:

		// Win32APIに渡すウィンドウプロシージャは通常のメンバ関数では渡せないため、
		// static関数として定義して呼び出しの入口にする
		static LRESULT CALLBACK CallWindowProcedure(const HWND   a_hwnd,
												    const UINT   a_message,
												    const WPARAM a_wPARAM,
												    const LPARAM a_lPARAM);

		LRESULT CALLBACK WindowProcedure(const HWND   a_hwnd,
										 const UINT   a_message,
										 const WPARAM a_wARAM,
										 const LPARAM a_lARAM);

		bool CreateWindowInstance(const std::wstring& a_windowClassName, const std::string& a_titleName);

		void SetupClientSize();

		void Release();

		void ApplyClientSizeFromWMSize(const UINT a_width, const UINT a_height, const WPARAM& a_wPARAM);

		bool IsValidClientSize(const Struct::ClientSize& a_clientSize) const;

		HINSTANCE FetchVALInstanceHandle() const;

		DWORD FetchVALWindowStyle() const;

		static constexpr LRESULT k_windowProcedureHandledResult = 0;
		
		static constexpr DWORD k_generalWindowStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME;

		static constexpr std::uint32_t k_invalidWindowClientWidth  = 0U;
		static constexpr std::uint32_t k_invalidWindowClientHeight = 0U;
		
		static constexpr UINT k_windowStyle = CS_HREDRAW | CS_VREDRAW;

		static constexpr UINT k_timeResolutionMS = 1U;

		static constexpr UINT k_msgFilterMIN          = 0U;
		static constexpr UINT k_msgFilterMAX          = 0U;
		static constexpr UINT k_wmCreateHandledResult = 0U;

		static constexpr LONG k_clientRectOriginX = 0L;
		static constexpr LONG k_clientRectOriginY = 0L;

		// ウィンドウのタイトルバー、最小化、最大化機能を持たせウィンドウのサイズ変更機能を除外したスタイル
		static constexpr std::wstring_view k_windowInstancePropertyName = L"GameWindowInstance";

		static constexpr int k_classExtraBytes  = 0;
		static constexpr int k_windowExtraBytes = 0;

		static constexpr int k_defaultWindowPositionX = 0;
		static constexpr int k_defaultWindowPositionY = 0;

		static constexpr int k_quitExitCode = 0;

		const std::filesystem::path k_configFileIOPath = "Asset/Data/CONFIG/Window/WindowCONFIG.json";

		HWND m_hwnd = nullptr;

		Converter::WindowJsonConverter m_windowJsonConverter = {};

		Struct::WindowCONFIG		m_windowCONFIG  = {};
		Struct::WindowResizeRequest m_resizeRequest = {};
	};
}