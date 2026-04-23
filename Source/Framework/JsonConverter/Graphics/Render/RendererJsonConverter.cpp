#include "RendererJsonConverter.h"

void FWK::JsonConverter::RendererJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null()) { return; }

	DeserializeFrameResourceList(a_rootJson["FrameResourceList"], a_renderer);
	DeserializeRootSignatureMap (a_rootJson["RootSignatureMap"],  a_renderer);
}

nlohmann::json FWK::JsonConverter::RendererJsonConverter::Serialize(const Graphics::Renderer& a_renderer) const
{
	nlohmann::json l_rootJson = {};

	l_rootJson["FrameResourceList"] = SerializeFrameResourceList(a_renderer);
	l_rootJson["RootSignatureMap"]  = SerializeRootSignatureMap (a_renderer);
	
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
			  auto l_rootSignature = Graphics::RootSignature      ();
		const auto l_tag           = Utility::Json::DeserializeTag(l_json, "RootSignatureTag");

		if (l_json.contains("RootSignature"))
		{
			l_rootSignature.Deserialize(l_json["RootSignature"]);
		}

		a_renderer.AddRootSignature(l_rootSignature, l_tag);
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

		l_json["RootSignature"] = l_value.Serialize();

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}