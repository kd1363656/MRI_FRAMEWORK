#pragma once

namespace FWK
{
	// 型情報を指定しなくてもTypeInfoを取得できるレジストリークラス
	class TypeInfoRegistry final : public FWK::SingletonBase<FWK::TypeInfoRegistry>
	{
	public:

		void Register(const FWK::TypeInfo& a_info);

		// 指定した基底クラス、基底構造体を継承しているTypeInfo全てを取得
		template <typename Type>
		std::vector<const FWK::TypeInfo*> FetchTypeInfoDerivedFromBaseList() const
		{
			std::vector<const TypeInfo*> l_list = {};

			for (const auto& [l_key, l_value] : m_allTypeInfoIDMap)
			{
				if (!l_value || 
					!Utility::TypeInfo::IsDerivedFrom(*l_value, FWK::GetTypeInfo<Type>())) { continue; }

				l_list.emplace_back(l_value);
			}

			return l_list;
		}

		const FWK::TypeInfo* FetchTypeInfoByID  (const std::uint32_t     a_id)   const;
		const FWK::TypeInfo* FetchTypeInfoByName(const std::string_view& a_name) const;

	private:

		// 文字列がキーのマップはjsonのシリアライズ時に使用
		std::unordered_map<std::uint32_t,    const TypeInfo* const>													m_allTypeInfoIDMap   = {};
		std::unordered_map<std::string_view, const TypeInfo* const, FWK::CommonStruct::StringHash, std::equal_to<>> m_allTypeInfoNameMap = {};

		//=========================
		// シングルトン
		//=========================
		friend class FWK::SingletonBase<FWK::TypeInfoRegistry>;

		TypeInfoRegistry ()          = default;
		~TypeInfoRegistry() override = default;
	};
}