#include "SceneViewEditorWindow.h"

void FWK::Editor::SceneViewEditorWindow::Draw()
{
	const auto& l_windowName = GetTypeINFO().k_name.data();

	ImGui::Begin(l_windowName);



	ImGui::End();
}