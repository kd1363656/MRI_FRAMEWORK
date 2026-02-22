#pragma once

namespace FWK::Utility::Json
{
	// jsonに他のjsonの内容をコピーする
	inline void UpdateJson(nlohmann::json& a_targetJson, const nlohmann::json& a_patchJson)
	{
		if (a_patchJson.is_null()) { return; }

		a_targetJson.update(a_patchJson);
	}

	inline std::uint32_t DeserializeTag(const nlohmann::json& a_json)
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

	inline nlohmann::json SerializeTag(const TypeDEF::Tag a_tag)
	{
		const auto& l_registry = FWK::TypeInfoRegistry::GetInstance();
		const auto* l_typeInfo = l_registry.FetchTypeInfoByID      (a_tag);

		if (!l_typeInfo)
		{
			return
			{
				{ "TagName", std::string() }
			};
		}

		return
		{
			{ "TagName", l_typeInfo->k_name.data() }
		};
	}
}