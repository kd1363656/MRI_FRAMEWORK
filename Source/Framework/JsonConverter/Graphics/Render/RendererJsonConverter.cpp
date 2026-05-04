#include "RendererJsonConverter.h"

void FWK::JsonConverter::RendererJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null()) { return; }

	if (a_rootJson.contains(k_frameResourceListJsonKey))
	{
		DeserializeFrameResourceList(a_rootJson[k_frameResourceListJsonKey], a_renderer);
	}

	if (a_rootJson.contains(k_rootSignatureMapJsonKey))
	{
		DeserializeRootSignatureMap(a_rootJson[k_rootSignatureMapJsonKey], a_renderer);
	}

	if (a_rootJson.contains(k_pipelineStateMapJsonKey))
	{
		DeserializePipelineStateMap(a_rootJson[k_pipelineStateMapJsonKey], a_renderer);
	}

	if (a_rootJson.contains(k_drawCommandJsonKey))
	{
		DeserializeDrawCommand(a_rootJson[k_drawCommandJsonKey], a_renderer);
	}
}

nlohmann::json FWK::JsonConverter::RendererJsonConverter::Serialize(const Graphics::Renderer& a_renderer) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson[k_frameResourceListJsonKey] = SerializeFrameResourceList(a_renderer);
	l_rootJson[k_rootSignatureMapJsonKey]  = SerializeRootSignatureMap (a_renderer);
	l_rootJson[k_pipelineStateMapJsonKey]  = SerializePipelineStateMap (a_renderer);
	l_rootJson[k_drawCommandJsonKey]       = SerializeDrawCommand      (a_renderer);

	return l_rootJson;
}

void FWK::JsonConverter::RendererJsonConverter::DeserializeFrameResourceList(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	const auto& l_arrayNum = a_rootJson.size();

	auto& l_resourceList = a_renderer.GetMutableREFFrameResourceList();

	l_resourceList.resize(l_arrayNum);
}
void FWK::JsonConverter::RendererJsonConverter::DeserializeRootSignatureMap(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
			  Graphics::RootSignature l_rootSignature = {};
		const auto					  l_tag           = Utility::Json::DeserializeTag(l_json, k_rootSignatureTagJsonKey);

		if (l_json.contains(k_rootSignatureJsonKey))
		{
			l_rootSignature.Deserialize(l_json[k_rootSignatureJsonKey]);
		}

		a_renderer.AddRootSignature(l_rootSignature, l_tag);
	}	
}
void FWK::JsonConverter::RendererJsonConverter::DeserializePipelineStateMap(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
			  Graphics::PipelineState l_pipelineState = {};
		const auto					  l_tag           = Utility::Json::DeserializeTag(l_json, k_pipelineStateTagJsonKey);

		if (l_json.contains(k_pipelineStateJsonKey))
		{
			l_pipelineState.Deserialize(l_json[k_pipelineStateJsonKey]);
		}

		a_renderer.AddPipelineState(l_pipelineState, l_tag);
	}	
}
void FWK::JsonConverter::RendererJsonConverter::DeserializeDrawCommand(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }
	
	for (const auto& l_json : a_rootJson)
	{
		std::shared_ptr<Graphics::IDrawCommand> l_drawCommand = nullptr;
		
		// ファクトリーからDrawCommandを作成
		Utility::Json::DeserializeInstanceType<TypeAlias::ShaderFactoryDrawCommand>(l_json, k_drawCommandTypeNameJsonKey, l_drawCommand);
		
		// 作成に成功していれば中身にポインタがしっかり入っているのでデシリアライズを行う
		if (l_drawCommand)
		{
			a_renderer.AddDrawCommandList(l_drawCommand);
			a_renderer.AddDrawCommandMap (l_drawCommand, l_drawCommand->GetRuntimeTypeINFO().k_staticTypeID);
		}
	}
}

nlohmann::json FWK::JsonConverter::RendererJsonConverter::SerializeFrameResourceList(const Graphics::Renderer& a_renderer) const
{
	// フレームリソースリストの保存
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_frameResourceList = a_renderer.GetREFResourceList();

	// 数だけ記録するため空のjsonを保存させる
	for (const auto& l_frameResource : l_frameResourceList)
	{
		nlohmann::json l_json = {};

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::JsonConverter::RendererJsonConverter::SerializeRootSignatureMap(const Graphics::Renderer& a_renderer) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_rootSignatureMap = a_renderer.GetREFRootSignatureMap();

	for (const auto& [l_key, l_value] : l_rootSignatureMap)
	{
		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_key, k_rootSignatureTagJsonKey));
		l_json[k_rootSignatureJsonKey] = l_value.Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::JsonConverter::RendererJsonConverter::SerializePipelineStateMap(const Graphics::Renderer& a_renderer) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_pipelineStateMap = a_renderer.GetREFPipelineStateMap();

	for (const auto& [l_key, l_value] : l_pipelineStateMap)
	{
		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_key, k_pipelineStateTagJsonKey));
		l_json[k_pipelineStateJsonKey] = l_value.Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::JsonConverter::RendererJsonConverter::SerializeDrawCommand(const Graphics::Renderer& a_renderer) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_drawCommandList = a_renderer.GetREFDrawCommandList();

	// 実行順序をそのまま格納するためにstd::unordered_mapではなくstd::vectorを
	// 使ってSerializeを行う
	for (const auto& l_drawCommand : l_drawCommandList)
	{
		l_rootJsonArray.emplace_back(Utility::Json::SerializeInstanceType(l_drawCommand, k_drawCommandTypeNameJsonKey));
	}

	return l_rootJsonArray;
}