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
	inline bool IsArray(const nlohmann::json& a_json, const std::string_view& a_key = {})
	{
		// jsonが空ならreturn
		if (a_json.is_null()) { return false; }

		// 文字列が空ならa_jsonが配列かどうかの結果を返す
		if (a_key.empty()) { return a_json.is_array(); }

		// もしキーが含まれているがa_jsonに含まれていな得ればreturn
		if (!a_json.contains(a_key)) { return false; }

		// キーが含まれていればa_json[a_key.data()]が配列なのかどうかの結果を返す
		return a_json[a_key].is_array();
	}

	inline TypeAlias::Math::Color DeserializeColor(const nlohmann::json& a_json) 
	{
		return TypeAlias::Math::Color    
		{ 
			a_json.value("X", Constant::k_noColor.R()), 
			a_json.value("Y", Constant::k_noColor.G()), 
			a_json.value("Z", Constant::k_noColor.B()),
			a_json.value("W", Constant::k_noColor.A())
		}; 
	}
	inline nlohmann::json SerializeColor(const TypeAlias::Math::Color& a_color) 
	{ 
		return nlohmann::json
		{ 
			{"X" , a_color.x }, 
			{"Y" , a_color.y }, 
			{"Z" , a_color.z },
			{"W" , a_color.w }
		}; 
	}

	inline TypeAlias::TypeTag DeserializeTag(const nlohmann::json& a_json, const std::string_view& a_key)
	{
		if (a_json.is_null()) { return Constant::k_invalidStaticTypeID; }

		// jsonからTag名を取得
		const auto& l_registry = TypeINFORegistry::GetInstance();
		const auto& l_tagName  = a_json.value                 (a_key, std::string());

		// 取得したタグ名から型情報を検索
		const auto* l_typeINFO = l_registry.FindTypeINFOByName(l_tagName);

		// もし型情報がnullptrを示すということはTypeTagの取得に失敗したということ
		if (!l_typeINFO) { return Constant::k_invalidStaticTypeID; }

		return l_typeINFO->k_staticTypeID;
	}

	inline nlohmann::json SerializeTag(const TypeAlias::TypeTag  a_typeTag, const std::string_view& a_key)
	{
		const auto& l_registry = TypeINFORegistry::GetInstance();
		const auto* l_typeINFO = l_registry.FindTypeINFOByID  (a_typeTag);

		if (!l_typeINFO)
		{
			return nlohmann::json
			{ 
				{ a_key, std::string() } 
			};
		}

		return nlohmann::json
		{
			{ a_key, l_typeINFO->k_name }
		};
	}

	// ※注意 : FWK_REGISTER_FACTORY_METHODマクロを使用していないと使用できないので注意
	template <typename FactoryType , typename Type>
		requires Concept::IsSmartPTRConcept<Type>
	inline void DeserializeInstanceType(const nlohmann::json& a_json, const std::string_view& a_key, Type& a_instance)
	{
		if (a_json.is_null()) { return; }

		const auto& l_factory = FactoryType::GetInstance();

		const auto& l_createName = a_json.value(a_key, std::string());

		a_instance = l_factory.Create(l_createName);
	}

	// インスタンスから型名を取得し保存する
	// ※注意 : FWK_REGISTER_FACTORY_METHODマクロを使用していないと使用できないので注意
	template <typename Type>
		requires Concept::IsSmartPTRConcept<Type>
	inline nlohmann::json SerializeInstanceType(const Type& a_instance, const std::string_view& a_key)
	{
		if (!a_instance) { return nlohmann::json(); }

		auto l_rootJson = nlohmann::json();

		l_rootJson[a_key] = a_instance->GetREFRuntimeTypeINFO().k_name;

		return l_rootJson;
	}
}