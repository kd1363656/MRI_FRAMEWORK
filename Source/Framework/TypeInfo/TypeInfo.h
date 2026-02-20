#pragma once

namespace FWK
{
	struct TypeInfo final
	{
		const FWK::TypeInfo* const k_baseInfo = nullptr;
		const std::string_view     k_name     = std::string_view{};
		const std::uint32_t	       k_id       = FWK::CommonConstant::k_invalidStaticID;
	};

	// 関数の明示的特殊化を行うことで使用可能になる(明示的特殊化を行わないとコンパイルエラー)
	// 書き方的にテンプレートの特殊化宣言はクラス内部で書けないためクラス定義後に書く必要がある(クラスの完全な宣言がクラス内部で行うことができないため)
	// クラスで自身の"TypeInfo"にアクセスできる関数が作りたいなら定義はヘッダーに、関数の実装は"cpp"で行う。
	template <typename Type>
	const TypeInfo& GetTypeInfo() = delete; // 型が見つからなかったときの関数を削除することで明示的特殊化を強制

	// 型情報がマクロで登録されたかどうかを判断する
	// もし型情報のエラーが出たときのデバック用
	template <typename Type>
	struct IsTypeInfoRegistered : std::false_type {};
}

// 型情報登録マクロ(親なし)
#define FWK_DEFINE_TYPE_INFO_ROOT(Type)					   \
namespace FWK											   \
{														   \
	template<>											   \
	struct IsTypeInfoRegistered<Type> : std::true_type {}; \
														   \
	template<>											   \
	inline const TypeInfo& GetTypeInfo<Type>()			   \
	{													   \
		static const TypeInfo l_info =					   \
		{											       \
			nullptr,								       \
			#Type,									       \
			StaticID::GetTypeID<Type>()					   \
		};												   \
		return l_info;									   \
	}													   \
}

// 型情報登録マクロ(親アリ)
#define FWK_DEFINE_TYPE_INFO(Type, BaseType)														\
namespace FWK																					\
{																								\
	template<>																					\
	struct IsTypeInfoRegistered<Type> : std::true_type {};										\
																								\
	template<>																					\
	inline const TypeInfo& GetTypeInfo<Type>()													\
	{																							\
		const TypeInfo& l_baseTypeInfo = GetTypeInfo<BaseType>();								\
		assert(&l_baseTypeInfo && "基底クラスの型情報が未登録です。コードを見直してください");	\
																								\
		static const TypeInfo l_info =															\
		{																						\
			&l_baseTypeInfo,																	\
			#Type,																				\
			StaticID::GetTypeID<Type>()															\
		};																						\
		return l_info;																			\
	}																							\
}