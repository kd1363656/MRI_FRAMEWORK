#include "RendererJsonConverter.h"

void FWK::Converter::RendererJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::Renderer& a_renderer) const
{
	if (a_rootJson.is_null()) { return; }

	if (Utility::Json::IsArray(a_rootJson, "FrameResourceList"))
	{
		const auto& l_arrayNum = a_rootJson["FrameResourceList"].size();

		// フレームリソースの数分追加する
		for (size_t l_i = 0ULL; l_i < l_arrayNum; ++l_i)
		{
			auto l_frameResource = Graphics::FrameResource();

			a_renderer.AddFrameResource(l_frameResource);
		}
	}
}

nlohmann::json FWK::Converter::RendererJsonConverter::Serialize(const Graphics::Renderer& a_renderer) const
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