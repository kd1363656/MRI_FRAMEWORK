#pragma once

namespace FWK::Utility::Json
{
	// jsonに他のjsonの内容をコピーする
	inline void UpdateJson(nlohmann::json& a_targetJson, const nlohmann::json& a_patchJson)
	{
		if (a_patchJson.is_null()) { return; }

		a_targetJson.update(a_patchJson);
	}

	// 読み込んだjsonが配列かどうかを確認
	inline bool IsArray(const nlohmann::json& a_json , const std::string_view a_key)
	{
		// 文字列が空でなくそのキーが含まれているかをチェック
		if (a_json.is_null() || 
			a_key.empty()    || 
			!a_json.contains(a_key.data())) { return false; }

		// もし文字列が空(デフォルト引数)ならjson自体が配列なのでキーを含めずに配列化チェック
		// そうでなければキーに連続した配列なのでキーを含めた配列チェックを行う
		return a_key.empty() ? a_json.is_array() : a_json[a_key.data()].is_array();
	}


	inline ::Tag DeserializeTag(const nlohmann::json& a_json)
	{
		if (a_json.is_null()) { return FWK::CommonConstant::k_invalidStaticID; }

		const auto& l_registry = TypeInfoRegistry::GetInstance();
		const auto& l_tagName  = a_json.value                 ("TagName", std::string());

		const FWK::TypeInfo* l_typeInfo = l_registry.FetchTypeInfoByName(l_tagName);
		if (!l_typeInfo)
		{
			return FWK::CommonConstant::k_invalidStaticID;
		}

		return l_typeInfo->k_id;
	}

	inline nlohmann::json SerializeTag(const ::Tag a_tag)
	{
		const auto& l_registry = FWK::TypeInfoRegistry::GetInstance();
		const auto* l_typeInfo = l_registry.FetchTypeInfoByID      (a_tag);

		if (!l_typeInfo)
		{
			return 
			nlohmann::json({
				{ "TagName", std::string() } 	
			});
		}

		return 
		nlohmann::json({ 
			{ "TagName", l_typeInfo->k_name.data() } 
		});
	}
}