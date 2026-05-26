#include "RenderGraphJsonConverter.h"

void FWK::Converter::RenderGraphJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderGraph& a_renderGraph) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_passListJsonKey))
	{
		DeserializePassList(a_rootJson[k_passListJsonKey], a_renderGraph);
	}
}

nlohmann::json FWK::Converter::RenderGraphJsonConverter::Serialize(const Graphics::RenderGraph& a_renderGraph) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_passListJsonKey] = SerializePassList(a_renderGraph);

	return l_rootJson;
}

void FWK::Converter::RenderGraphJsonConverter::DeserializePassList(const nlohmann::json& a_rootJson, Graphics::RenderGraph& a_renderGraph) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	a_renderGraph.INIT();

	for (const auto& l_json : a_rootJson)
	{
		std::unique_ptr<Graphics::IRenderGraphPass> l_renderGraphPass = nullptr;

		// ファクトリーからRenderGraphPassを作成する
		Utility::Json::DeserializeInstanceType<TypeAlias::ShaderFactoryRenderGraphPass>(l_json, k_renderGraphPassTypeNameJsonKey, l_renderGraphPass);

		if (!l_renderGraphPass)
		{
			assert(false && "RenderGraphPassの生成に失敗しました。");
			continue;
		}

		a_renderGraph.AddPass(std::move(l_renderGraphPass));
	}
}
nlohmann::json FWK::Converter::RenderGraphJsonConverter::SerializePassList(const Graphics::RenderGraph& a_renderGraph) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_passList = a_renderGraph.GetREFPassList();

	for (const auto& l_pass : l_passList)
	{
		if (!l_pass) { continue; }

		l_rootJsonArray.emplace_back(Utility::Json::SerializeInstanceType(l_pass, k_renderGraphPassTypeNameJsonKey));
	}

	return l_rootJsonArray;
}