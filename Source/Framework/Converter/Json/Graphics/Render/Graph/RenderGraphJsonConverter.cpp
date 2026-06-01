#include "RenderGraphJsonConverter.h"

void FWK::Converter::RenderGraphJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RenderGraph& a_renderGraph) const
{
	if (a_rootJson.is_null()) { return; }

	// レンダーパスのシリアライズ
	if (a_rootJson.contains(k_passListJsonKey))
	{
		DeserializePassList(a_rootJson[k_passListJsonKey], a_renderGraph);
	}

	// Pass全体で共有するDrawRequestのデシリアライズ
	if (a_rootJson.contains(k_drawRequestPassListJsonKey))
	{
		DeserializeDrawRequestPassList(a_rootJson[k_drawRequestPassListJsonKey], a_renderGraph);
	}

	// 一つの描画対象ごとに違うDrawRequestのデシリアライズ
	if (a_rootJson.contains(k_drawRequestPerObjectListJsonKey))
	{
		DeserializeDrawRequestPerObjectList(a_rootJson[k_drawRequestPerObjectListJsonKey], a_renderGraph);
	}
}

nlohmann::json FWK::Converter::RenderGraphJsonConverter::Serialize(const Graphics::RenderGraph& a_renderGraph) const
{
	nlohmann::json l_rootJson = {};

	// レンダーパスのシリアライズ
	l_rootJson[k_passListJsonKey] = SerializePassList(a_renderGraph);

	// Pass単位のDrawRequestを保存する
	l_rootJson[k_drawRequestPassListJsonKey] = SerializeDrawRequestPassList(a_renderGraph);

	// Object単位のDrawRequestを保存する
	l_rootJson[k_drawRequestPerObjectListJsonKey] = SerializeDrawRequestPerObjectList(a_renderGraph);

	return l_rootJson;
}

void FWK::Converter::RenderGraphJsonConverter::DeserializePassList(const nlohmann::json& a_rootJson, Graphics::RenderGraph& a_renderGraph) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		std::unique_ptr<Graphics::IRenderGraphPass> l_renderGraphPass = nullptr;

		// ファクトリーからRenderGraphPassを作成する
		Utility::Json::DeserializeInstanceType<TypeAlias::SharedFactoryRenderGraphPass>(l_json, k_renderGraphPassTypeNameJsonKey, l_renderGraphPass);

		if (!l_renderGraphPass)
		{
			assert(false && "RenderGraphPassの生成に失敗しました。");
			continue;
		}

		a_renderGraph.AddPass(std::move(l_renderGraphPass));
	}
}
void FWK::Converter::RenderGraphJsonConverter::DeserializeDrawRequestPassList(const nlohmann::json& a_rootJson, Graphics::RenderGraph& a_renderGraph) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		std::shared_ptr<Graphics::DrawRequestPassBase> l_drawRequestPass = nullptr;

		// ファクトリーからDrawRequestPassを作成する。
		Utility::Json::DeserializeInstanceType<TypeAlias::SharedFactoryDrawRequestPass>(l_json, k_drawRequestPassTypeNameJsonKey, l_drawRequestPass);

		if (!l_drawRequestPass)
		{
			assert(false && "DrawRequestPassの生成に失敗しました。");
			continue;
		}

		a_renderGraph.AddDrawRequestPass(l_drawRequestPass);
	}
}
void FWK::Converter::RenderGraphJsonConverter::DeserializeDrawRequestPerObjectList(const nlohmann::json& a_rootJson, Graphics::RenderGraph& a_renderGraph) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		std::shared_ptr<Graphics::DrawRequestPerObjectBase> l_drawRequestPerObject = nullptr;

		// ファクトリーからDrawRequestPassを作成する。
		Utility::Json::DeserializeInstanceType<TypeAlias::SharedFactoryDrawRequestPerObject>(l_json, k_drawRequestPerObjectTypeNameJsonKey, l_drawRequestPerObject);

		if (!l_drawRequestPerObject)
		{
			assert(false && "DrawRequestPassの生成に失敗しました。");
			continue;
		}

		a_renderGraph.AddDrawRequestPerObject(l_drawRequestPerObject);
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
nlohmann::json FWK::Converter::RenderGraphJsonConverter::SerializeDrawRequestPassList(const Graphics::RenderGraph& a_renderGraph) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_drawRequestPassList = a_renderGraph.GetREFDrawRequestPassList();

	for (const auto& l_drawRequestPass : l_drawRequestPassList)
	{
		if (!l_drawRequestPass) { continue; }

		l_rootJsonArray.emplace_back(Utility::Json::SerializeInstanceType(l_drawRequestPass, k_drawRequestPassTypeNameJsonKey));
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::Converter::RenderGraphJsonConverter::SerializeDrawRequestPerObjectList(const Graphics::RenderGraph& a_renderGraph) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_drawRequestPerObjectList = a_renderGraph.GetREFDrawRequestPerObjectList();

	for (const auto& l_drawRequestPerObject : l_drawRequestPerObjectList)
	{
		if (!l_drawRequestPerObject) { continue; }

		l_rootJsonArray.emplace_back(Utility::Json::SerializeInstanceType(l_drawRequestPerObject, k_drawRequestPerObjectTypeNameJsonKey));
	}

	return l_rootJsonArray;
}