#pragma once

// ※注意 : これらのマクロはすべて自動で型情報のゲッターと
// 型情報登録クラスに登録するがクラスや構造体内部で書く必要がある

// 型情報登録マクロ(親なし)
#define FWK_DEFINE_TYPE_INFO_ROOT(Type)																	\
public:																									\
	static const auto& GetTypeInfo()																	\
	{																									\
		static const auto l_typeInfo = FWK::TypeInfo(nullptr, #Type, FWK::StaticID::GetTypeID<Type>()); \
		return l_typeInfo;																				\
	};																									\
																										\
private:																								\
	class RegisterTypeInfo																				\
	{																									\
	public:																								\
		RegisterTypeInfo()																				\
		{																								\
			FWK::TypeInfoRegistry::GetInstance().Register(GetTypeInfo());								\
		}																								\
																										\
		~RegisterTypeInfo() = default;																	\
	};																									\
	inline static RegisterTypeInfo k_autoRegister = RegisterTypeInfo();

// 型情報登録マクロ(親あり)
#define FWK_DEFINE_TYPE_INFO(Type, BaseType)																				   \
public:																														   \
	static const auto& GetTypeInfo()																						   \
	{																														   \
		static const auto l_typeInfo = FWK::TypeInfo(&BaseType::GetTypeInfo(), #Type, FWK::StaticID::GetTypeID<Type>());       \
		return l_typeInfo;																									   \
	};																														   \
																															   \
private:																													   \
	class RegisterTypeInfo																									   \
	{																														   \
	public:																													   \
		RegisterTypeInfo()																									   \
		{																													   \
			FWK::TypeInfoRegistry::GetInstance().Register(GetTypeInfo());													   \
		}																													   \
																															   \
		~RegisterTypeInfo() = default;																						   \
	};																														   \
	inline static RegisterTypeInfo k_autoRegister = RegisterTypeInfo();