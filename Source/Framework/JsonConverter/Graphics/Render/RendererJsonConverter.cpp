#include "RendererJsonConverter.h"

void FWK::JsonConverter::RendererJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null()) { return; }

	if (Utility::Json::IsArray(a_rootJson, "FrameResourceList"))
	{
		const auto& l_arrayNum = a_rootJson["FrameResourceList"].size();

		auto& l_resourceList = a_renderer.GetMutableREFFrameResourceList();

		l_resourceList.resize(l_arrayNum);
	}
}

nlohmann::json FWK::JsonConverter::RendererJsonConverter::Serialize(const Graphics::Renderer& a_renderer) const
{
	nlohmann::json l_rootJson = {};

	// フレームリソースリストの保存
	{
		auto l_jsonArray = nlohmann::json::array();

		const auto& l_frameResourceList = a_renderer.GetREFResourceList();

		// 数だけ記録するため空のjsonを保存させる
		for (std::size_t l_i = 0ULL; l_i < l_frameResourceList.size(); ++l_i)
		{
			nlohmann::json l_json = {};

			l_jsonArray.emplace_back(l_json);
		}

		l_rootJson["FrameResourceList"] = l_jsonArray;
	}

	return l_rootJson;
}