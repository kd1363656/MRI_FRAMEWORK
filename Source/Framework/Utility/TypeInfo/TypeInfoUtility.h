#pragma once

namespace FWK
{
	class TypeInfoRegistry;
}

namespace FWK::Utility::TypeInfo
{
	void RegisterAll();

	void RegisterTag(FWK::TypeInfoRegistry& a_registry);

	void RegisterWindowTypeTag(FWK::TypeInfoRegistry& a_registry);

	// Žw’è‚³‚ê‚½Œ^‚ðŒp³‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©‚ðŠm”FAŒp³‰ñ”‚ª‘½‚¯‚ê‚Î‘½‚¢‚Ù‚Çˆ—‚ª’x‚­‚È‚é
	inline bool IsDerivedFrom(const FWK::TypeInfo& a_typeInfo, const FWK::TypeInfo& a_baseTypeInfo)
	{
		const FWK::TypeInfo* l_currentInfo = &a_typeInfo;

		// ”O‚Ì‚½‚ßƒkƒ‹ƒ`ƒFƒbƒN(ŽQÆ‚È‚Ì‚Ånullptr‚Íâ‘Î‚É‚ ‚è‚¦‚È‚¢‚ª)
		if (!l_currentInfo)
		{
			assert(false && "Œ^î•ñ‚ÌƒAƒhƒŒƒX‚ª\"nullptr\"‚Å‚µ‚½AŒ^î•ñ’è‹`‚ðŒ©’¼‚µ‚Ä‚­‚¾‚³‚¢");
			return false;
		}

		// Ž©g‚ªnullptr‚ðŽ¦‚·‚Â‚Ü‚èe‚ð‘–¸’ª‚ð“¾‚½‚çwhile‚ð”²‚¯‚é
		while (l_currentInfo)
		{
			// Ž©g‚ÌŒ»Ý‚Ì"ID"‚Æe‚Ì"ID"‚ªˆê’v‚·‚ê‚Îtrue‚ð•Ô‚·
			if (l_currentInfo->k_id == a_baseTypeInfo.k_id)
			{
				return true;
			}

			// Ž©g‚Ìƒƒ“ƒo‚ÉŠÜ‚Ü‚ê‚ée•ƒ‚ðŽŸ‚Ìwhile‚Å‘–¸‚·‚é‘ÎÛ‚Æ‚µ‚ÄŽw’è(Œp³ƒcƒŠ[‚ð’H‚é)
			l_currentInfo = l_currentInfo->k_baseInfo;
		}

		return false;
	}
}