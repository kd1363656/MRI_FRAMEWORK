#pragma once

namespace FWK::Editor
{
	class EditorManager;
}

namespace FWK::Converter
{
	class EditorManagerJsonConverter final
	{
	public:

		 EditorManagerJsonConverter() = default;
		~EditorManagerJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Editor::EditorManager& a_editorManager) const;

		nlohmann::json Serialize(const Editor::EditorManager& a_editorManager) const;
	};
}