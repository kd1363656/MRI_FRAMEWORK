#include "TypeInfoUtility.h"

void FWK::Utility::TypeInfo::RegisterAll()
{
	auto& l_registry = FWK::TypeInfoRegistry::GetInstance();

	RegisterTag(l_registry);
}

void FWK::Utility::TypeInfo::RegisterTag(FWK::TypeInfoRegistry& a_registry)
{
	RegisterWindowTypeTag(a_registry);
}

void FWK::Utility::TypeInfo::RegisterWindowTypeTag(FWK::TypeInfoRegistry& a_registry)
{
	a_registry.Register(FWK::GetTypeInfo<FWK::Tag::WindowStyleTagBase>      ());
	a_registry.Register(FWK::GetTypeInfo<FWK::Tag::WindowStyleNormalTag>    ());
	a_registry.Register(FWK::GetTypeInfo<FWK::Tag::WindowStyleFullScreenTag>());
}