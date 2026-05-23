#include "EditorManager.h"

FWK::Editor::EditorManager::EditorManager() = default;
FWK::Editor::EditorManager::~EditorManager()
{
	Release();
}

void FWK::Editor::EditorManager::Init(const HWND& a_hwnd)
{
	//// IMGUIのバージョンをチェックしIMGUIのセットアップをする
	//IMGUI_CHECKVERSION  ();
	//ImGui::CreateContext();

	//// IMGUIDockingの導入
	//ImGuiIO& l_io = ImGui::GetIO();

	//// IMGUIをウィンドウの外側に配置しても独立したOSウィンドウとして
	//// 表示を可能に、ウィンドウを結合、分割を可能にするフラグを合成する
	//l_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//l_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//// ImGuiのカラーを設定
	//ImGui::StyleColorsDark();
}

void FWK::Editor::EditorManager::DrawEditor()
{

}

void FWK::Editor::EditorManager::Release() const
{

}