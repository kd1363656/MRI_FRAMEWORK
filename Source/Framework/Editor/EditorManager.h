#pragma once

namespace FWK::Editor
{
	class EditorManager final : public SingletonBase<EditorManager>
	{
	public:

		void Init(const HWND& a_hwnd);

		void DrawEditor();

	private:

		void Release() const;

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<EditorManager>;

		 EditorManager();
		~EditorManager() override;
	};
}