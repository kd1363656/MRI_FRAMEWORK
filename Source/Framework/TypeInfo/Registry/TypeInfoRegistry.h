#pragma once

namespace FWK
{
	// 型情報を指定しなくてもTypeInfoを取得できるようにするためのレジストリークラス
	class TypeInfoRegistry final : public SingletonBase<TypeInfoRegistry>
	{
	private:

		using AllTypeInfoIDMap = std::unordered_map<std::uint32_t, const CommonStruct::TypeInfo* const>;

		// k_nameは静的寿命であることを前提にstd::string_viewをキーとして使用
		using AllTypeInfoNameMap = std::unordered_map<std::string_view, const CommonStruct::TypeInfo* const, CommonStruct::StringHash, std::equal_to<>>;

	public:

		void Register(const CommonStruct::TypeInfo& a_typeInfo);

		// 指定した基底クラス、基底構造体を継承しているTypeInfo全てを取得
		template <typename Type>
		std::vector<const CommonStruct::TypeInfo*> FindTypeInfoDerivedFromBaseList() const
		{
			auto l_list = std::vector<const CommonStruct::TypeInfo*>();

			for (const auto& [l_key, l_value] : m_allTypeInfoIDMap)
			{
				if (!l_value || 
					!Utility::TypeInfo::IsDerivedFrom(*l_value, Type::GetTypeInfo())) { continue; }

				l_list.emplace_back(l_value);
			}

			return l_list;
		}
		
		const CommonStruct::TypeInfo* FindTypeInfoByID(const std::uint32_t a_id)         const;
		const CommonStruct::TypeInfo* FindTypeInfoByName(const std::string_view& a_name) const;

	private:

		// 文字列がキーのマップはjsonのシリアライズ時に使用
		AllTypeInfoIDMap   m_allTypeInfoIDMap   = AllTypeInfoIDMap();
		AllTypeInfoNameMap m_allTypeInfoNameMap = AllTypeInfoNameMap();

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<FWK::TypeInfoRegistry>;

		TypeInfoRegistry()           = default;
		~TypeInfoRegistry() override = default;
	};
}