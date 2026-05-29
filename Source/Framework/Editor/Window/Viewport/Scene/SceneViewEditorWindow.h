#pragma once

namespace FWK::Editor
{
	class SceneViewEditorWindow final : public EditorWindowBase
	{
	public:

		 SceneViewEditorWindow()		  = default;
		~SceneViewEditorWindow() override = default;

		void Draw() override;

	private:


		FWK_DEFINE_TYPE_INFO(SceneViewEditorWindow, EditorWindowBase);
	};
}

FWK_REGISTER_FACTORY_METHOD(FWK::TypeAlias::SharedFactoryEditorWindow, FWK::Editor::SceneViewEditorWindow);