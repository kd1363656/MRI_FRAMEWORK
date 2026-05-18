#include "RendererJsonConverter.h"

void FWK::Converter::RendererJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null()) { return; }

	// フレームリソースリストのデシリアライズ
	if (a_rootJson.contains(k_frameResourceListJsonKey))
	{
		DeserializeFrameResourceList(a_rootJson[k_frameResourceListJsonKey], a_renderer);
	}

	// ルートシグネチャのデシリアライズ
	if (a_rootJson.contains(k_rootSignatureMapJsonKey))
	{
		DeserializeRootSignatureMap(a_rootJson[k_rootSignatureMapJsonKey], a_renderer);
	}

	// パイプラインステートのデシリアライズ
	if (a_rootJson.contains(k_pipelineStateMapJsonKey))
	{
		DeserializePipelineStateMap(a_rootJson[k_pipelineStateMapJsonKey], a_renderer);
	}

	// 描画コマンドのデシリアライズ
	if (a_rootJson.contains(k_drawCommandJsonKey))
	{
		DeserializeDrawCommand(a_rootJson[k_drawCommandJsonKey], a_renderer);
	}
}

nlohmann::json FWK::Converter::RendererJsonConverter::Serialize(const Graphics::Renderer& a_renderer) const
{
	nlohmann::json l_rootJson = {};

	// フレームリソースリストのシリアライズ
	l_rootJson[k_frameResourceListJsonKey] = SerializeFrameResourceList(a_renderer);

	// ルートシグネチャマップのシリアライズ
	l_rootJson[k_rootSignatureMapJsonKey] = SerializeRootSignatureMap(a_renderer);

	// パイプラインステートのシリアライズ
	l_rootJson[k_pipelineStateMapJsonKey] = SerializePipelineStateMap(a_renderer);

	// 描画コマンドリストのシリアライズ
	l_rootJson[k_drawCommandJsonKey] = SerializeDrawCommand(a_renderer);

	return l_rootJson;
}

void FWK::Converter::RendererJsonConverter::DeserializeFrameResourceList(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		if (!l_json.contains(k_frameResourceJsonKey)) { continue; }

		const auto& l_frameResource = std::make_shared<Graphics::FrameResource>();

		l_frameResource->Deserialize(l_json[k_frameResourceJsonKey]);

		a_renderer.AddFrameResource(l_frameResource);
	}
}
void FWK::Converter::RendererJsonConverter::DeserializeRootSignatureMap(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		const auto l_tag = Utility::Json::DeserializeTag(l_json, k_rootSignatureTagJsonKey);

		if (!l_json.contains(k_rootSignatureJsonKey)) { continue; }
		
		// ルートシグネチャのポインタを作成してレンダラー側に追加
		const auto& l_rootSignature = std::make_shared<Graphics::RootSignature>();

		l_rootSignature->Deserialize(l_json[k_rootSignatureJsonKey]);
		
		a_renderer.AddRootSignature(l_rootSignature, l_tag);
	}	
}
void FWK::Converter::RendererJsonConverter::DeserializePipelineStateMap(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }

	for (const auto& l_json : a_rootJson)
	{
		const auto l_tag = Utility::Json::DeserializeTag(l_json, k_pipelineStateTagJsonKey);

		if (!l_json.contains(k_pipelineStateJsonKey)) { continue; }
		
		const auto& l_pipelineState = std::make_shared<Graphics::PipelineState>();

		l_pipelineState->Deserialize(l_json[k_pipelineStateJsonKey]);
		
		a_renderer.AddPipelineState(l_pipelineState, l_tag);
	}	
}
void FWK::Converter::RendererJsonConverter::DeserializeDrawCommand(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }
	
	for (const auto& l_json : a_rootJson)
	{
		std::shared_ptr<Graphics::DrawCommandBase> l_drawCommand = nullptr;
		
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

nlohmann::json FWK::Converter::RendererJsonConverter::SerializeFrameResourceList(const Graphics::Renderer& a_renderer) const
{
	// フレームリソースリストの保存
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_frameResourceList = a_renderer.GetREFFrameResourceList();

	// 数だけ記録するため空のjsonを保存させる
	for (const auto& l_frameResource : l_frameResourceList)
	{
		if (!l_frameResource) { continue; }

		nlohmann::json l_json = {};

		l_json[k_frameResourceJsonKey] = l_frameResource->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::Converter::RendererJsonConverter::SerializeRootSignatureMap(const Graphics::Renderer& a_renderer) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_rootSignatureMap = a_renderer.GetREFRootSignatureMap();

	for (const auto& [l_tag, l_rootSignature] : l_rootSignatureMap)
	{
		if (!l_rootSignature) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson								   (l_json, Utility::Json::SerializeTag(l_tag, k_rootSignatureTagJsonKey));
		l_json[k_rootSignatureJsonKey] = l_rootSignature->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::Converter::RendererJsonConverter::SerializePipelineStateMap(const Graphics::Renderer& a_renderer) const
{
	auto l_rootJsonArray = nlohmann::json::array();

	const auto& l_pipelineStateMap = a_renderer.GetREFPipelineStateMap();

	for (const auto& [l_tag, l_pipelineState] : l_pipelineStateMap)
	{
		if (!l_pipelineState) { continue; }

		nlohmann::json l_json = {};

		Utility::Json::UpdateJson						           (l_json, Utility::Json::SerializeTag(l_tag, k_pipelineStateTagJsonKey));
		l_json[k_pipelineStateJsonKey] = l_pipelineState->Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::Converter::RendererJsonConverter::SerializeDrawCommand(const Graphics::Renderer& a_renderer) const
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