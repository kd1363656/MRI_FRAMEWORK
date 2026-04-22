#pragma once

namespace FWK
{
	// 型情報を指定しなくてもTypeInfoを取得できるようにするためのレジストリークラス
	class TypeINFORegistry final : public SingletonBase<TypeINFORegistry>
	{
	private:

		using AllTypeINFOStaticTypeIDMap = std::unordered_map<TypeAlias::StaticTypeID, const Struct::TypeINFO* const>;

		// k_nameは静的寿命であることを前提にstd::string_viewをキーとして使用
		using AllTypeINFONameMap = std::unordered_map<std::string_view, const Struct::TypeINFO* const, Struct::StringHash, std::equal_to<>>;

	public:

		void Register(const Struct::TypeINFO& a_typeINFO);

		// 指定した基底クラス、基底構造体を継承しているTypeInfo全てを取得
		template <typename Type>
		std::vector<const Struct::TypeINFO*> CollerctTypeINFODerivedFromBase() const
		{
			auto l_list = std::vector<const Struct::TypeINFO*>();

			for (const auto& [l_key, l_value] : m_allTypeINFOStaticTypeIDMap)
			{
				if (!l_value || 
					!Utility::TypeINFO::IsDerivedFromBase(*l_value, Type::GetTypeINFO()))
				{
					continue; 
				}

				l_list.emplace_back(l_value);
			}

			return l_list;
		}
		
		const Struct::TypeINFO* FindTypeINFOByID  (const TypeAlias::StaticTypeID a_staticTypeID) const;
		const Struct::TypeINFO* FindTypeINFOByName(const std::string_view&       a_name)         const;

	private:

		// 文字列がキーのマップはjsonのシリアライズ時に使用
		AllTypeINFOStaticTypeIDMap m_allTypeINFOStaticTypeIDMap = AllTypeINFOStaticTypeIDMap();
		AllTypeINFONameMap         m_allTypeINFONameMap         = AllTypeINFONameMap        ();

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<FWK::TypeINFORegistry>;

		 TypeINFORegistry()          = default;
		~TypeINFORegistry() override = default;
	};
}