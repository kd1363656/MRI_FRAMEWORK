#include "EditorManagerJsonConverter.h"

void FWK::Converter::EditorManagerJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Editor::EditorManager& a_editorManager) const
{
}

nlohmann::json FWK::Converter::EditorManagerJsonConverter::Serialize(const Editor::EditorManager& a_editorManager) const
{
	return nlohmann::json();
}
