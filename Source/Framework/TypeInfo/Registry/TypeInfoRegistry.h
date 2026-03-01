#pragma once

namespace FWK
{
	// 型情報を指定しなくてもTypeInfoを取得できるレジストリークラス
	class TypeInfoRegistry final : public FWK::SingletonBase<FWK::TypeInfoRegistry>
	{
	private:

		using AllTypeInfoIDMap = std::unordered_map<std::uint32_t, const TypeInfo* const>;

		// k_nameは静的寿命であることを前提にstd::string_viewをキーとして使用
		using AllTypeInfoNameMap = std::unordered_map<std::string_view, const TypeInfo* const, FWK::CommonStruct::StringHash, std::equal_to<>>; 

	public:

		void Register(const FWK::TypeInfo& a_info);

		// 指定した基底クラス、基底構造体を継承しているTypeInfo全てを取得
		template <typename Type>
		std::vector<const FWK::TypeInfo*> FetchTypeInfoDerivedFromBaseList() const
		{
			auto l_list = std::vector<const TypeInfo*>();

			for (const auto& [l_key, l_value] : m_allTypeInfoIDMap)
			{
				if (!l_value || 
					!Utility::TypeInfo::IsDerivedFrom(*l_value, Type::GetTypeInfo())) { continue; }

				l_list.emplace_back(l_value);
			}

			return l_list;
		}

		const FWK::TypeInfo* FetchTypeInfoByID  (const std::uint32_t     a_id)   const;
		const FWK::TypeInfo* FetchTypeInfoByName(const std::string_view& a_name) const;

	private:

		// 文字列がキーのマップはjsonのシリアライズ時に使用
		AllTypeInfoIDMap   m_allTypeInfoIDMap   = AllTypeInfoIDMap  ();
		AllTypeInfoNameMap m_allTypeInfoNameMap = AllTypeInfoNameMap();

		//=========================
		// シングルトン
		//=========================
		friend class FWK::SingletonBase<FWK::TypeInfoRegistry>;

		TypeInfoRegistry ()          = default;
		~TypeInfoRegistry() override = default;
	};
}