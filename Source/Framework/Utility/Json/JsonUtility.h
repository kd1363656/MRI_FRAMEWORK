#pragma once

namespace FWK::Utility::Json
{
	inline TypeAlias::TypeTag DeserializeTag(const nlohmann::json& a_json, const std::string_view& a_key)
	{
		if (a_json.is_null()) { return FWK::Constant::k_invalidStaticTypeID; }

		// jsonからTag名を取得
		const auto& l_registry = TypeINFORegistry::GetInstance();
		const auto& l_tagName  = a_json.value                 (a_key.data(), std::string());

		// 取得したタグ名から型情報を検索
		const auto* l_typeINFO = l_registry.FindTypeINFOByName(l_tagName);

		// もし型情報がnullptrを示すということはTypeTagの取得に失敗したということ
		if (!l_typeINFO) { return FWK::Constant::k_invalidStaticTypeID; }

		return l_typeINFO->k_staticTypeID;
	}

	inline nlohmann::json SerializeTag(const TypeAlias::TypeTag  a_typeTag, const std::string_view& a_key)
	{
		const auto& l_registry = TypeINFORegistry::GetInstance();
		const auto* l_typeINFO = l_registry.FindTypeINFOByID  (a_typeTag);

		if (!l_typeINFO)
		{
			return nlohmann::json{ 
				{ a_key.data(), std::string() } 
			};
		}

		return nlohmann::json{
			{ a_key.data(), l_typeINFO->k_name.data() }
		};
	}
}