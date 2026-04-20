#pragma once

// ※注意 : これらのマクロは型情報のゲッターと型情報登録クラスに自動で
// 登録する機能を提供するがクラスや構造体内部で書く必要がある。
// マクロを書く際には絶対にクラスの一番下に書くこと

// 型情報登録マクロ(基底)
#define FWK_DEFINE_TYPE_INFO_ROOT(Type)																						 \
public:																														 \
																															 \
	static const auto& GetTypeINFO()																						 \
	{																														 \
		static const auto l_typeINFO = FWK::Struct::TypeINFO(nullptr, FWK::StaticTypeIDGenerator::GetTypeID<Type>(), #Type); \
																															 \
		return l_typeINFO;																									 \
	};																														 \
																															 \
	virtual const FWK::Struct::TypeINFO& GetRuntimeTypeINFO() const { return GetTypeINFO(); }								 \
																															 \
private:																													 \
																															 \
	class RegisterTypeINFO																									 \
	{																														 \
	public:																													 \
																															 \
		RegisterTypeINFO()																									 \
		{																													 \
			FWK::TypeINFORegistry::GetInstance().Register(GetTypeINFO());													 \
		}																													 \
		~RegisterTypeINFO() = default;																						 \
	};																														 \
																															 \
	inline static const RegisterTypeINFO k_autoRegister = RegisterTypeINFO();

// 型情報登録マクロ(派生)
#define FWK_DEFINE_TYPE_INFO(Type, BaseType)																								  \
public:																																		  \
																																			  \
	static const auto& GetTypeINFO()																										  \
	{																																		  \
		static const auto l_typeINFO = FWK::Struct::TypeINFO(&BaseType::GetTypeINFO(), FWK::StaticTypeIDGenerator::GetTypeID<Type>(), #Type); \
																																			  \
		return l_typeINFO;																													  \
	};																																		  \
																																			  \
	const FWK::Struct::TypeINFO& GetRuntimeTypeINFO() const override { return GetTypeINFO(); }												  \
																																			  \
private:																																	  \
																																			  \
	class RegisterTypeINFO																													  \
	{																																		  \
	public:																																	  \
																																			  \
		RegisterTypeINFO()																													  \
		{																																	  \
			FWK::TypeINFORegistry::GetInstance().Register(GetTypeINFO());																	  \
		}																																	  \
		~RegisterTypeINFO() = default;																										  \
	};																																		  \
																																			  \
	inline static const RegisterTypeINFO k_autoRegister = RegisterTypeINFO();
