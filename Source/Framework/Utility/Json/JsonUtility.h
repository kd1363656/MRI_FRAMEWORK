#pragma once

namespace FWK::Utility::Json
{
	// 第1引数のjsonに第2引数のjsonの内容をコピーする
	inline void UpdateJson(nlohmann::json& a_targetJson, const nlohmann::json& a_patchJson)
	{
		if (a_patchJson.is_null()) { return; }

		a_targetJson.update(a_patchJson);
	}

	// 読み込んだjsonが配列かどうかを確認
	inline bool IsArray(const nlohmann::json& a_json , const std::string_view a_key)
	{
		// jsonが空ならreturn
		if (a_json.is_null())
		{
			return false;
		}

		// 文字列が空ならa_jsonが配列かどうかの結果を返す
		if (a_key.empty())
		{
			return a_json.is_array();
		}

		// もしキーが含まれているがa_jsonに含まれていな得ればreturn
		if (!a_json.contains(a_key.data()))
		{
			return false;
		}

		// キーが含まれていればa_json[a_key.data()]が配列なのかどうかの結果を返す
		return a_json[a_key.data()].is_array();
	}

	inline ::Tag DeserializeTag(const nlohmann::json& a_json)
	{
		if (a_json.is_null()) { return FWK::CommonConstant::k_invalidStaticID; }

		const auto& l_registry = TypeInfoRegistry::GetInstance();
		const auto& l_tagName  = a_json.value("TagName", std::string());

		const CommonStruct::TypeInfo* l_typeInfo = l_registry.FindTypeInfoByName(l_tagName);

		if (!l_typeInfo)
		{
			return FWK::CommonConstant::k_invalidStaticID;
		}

		return l_typeInfo->k_id;
	}

	inline nlohmann::json SerializeTag(const ::Tag a_tag)
	{
		const auto& l_registry = FWK::TypeInfoRegistry::GetInstance();
		const auto* l_typeInfo = l_registry.FindTypeInfoByID(a_tag);

		if (!l_typeInfo)
		{
			return nlohmann::json({
				{ "TagName", std::string() } 	
			});
		}

		return nlohmann::json({ 
			{ "TagName", l_typeInfo->k_name.data() } 
		});
	}
}