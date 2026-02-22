#include "RendererJsonConverter.h"

FWK::Converter::RendererJsonConverter::RendererJsonConverter(Render::Renderer& a_renderer) : 
	m_renderer(a_renderer)
{}
FWK::Converter::RendererJsonConverter::~RendererJsonConverter() = default;

void FWK::Converter::RendererJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	const UINT l_syncInterval = a_rootJson.value("SyncInterval", CommonConstant::k_defaultSyncInterval);

	m_renderer.SetSyncInterval(l_syncInterval);
}
nlohmann::json FWK::Converter::RendererJsonConverter::Serialize()
{
	auto l_rootJson = nlohmann::json{};

	l_rootJson["SyncInterval"] = m_renderer.GetSyncInterval();

	return l_rootJson;
}