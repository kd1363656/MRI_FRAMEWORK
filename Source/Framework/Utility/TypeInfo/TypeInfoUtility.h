#pragma once

namespace FWK::Utility::TypeInfo
{
	// 指定された型を継承しているかどうかを確認、継承回数が多ければ多いほど処理が遅くなる
	inline bool IsDerivedFrom(const FWK::TypeInfo& a_typeInfo, const FWK::TypeInfo& a_baseTypeInfo)
	{
		const FWK::TypeInfo* l_currentInfo = &a_typeInfo;

		// 自身がnullptrを示すつまり、a_typeInfoが継承している親まで走査し終えたらwhilte文を抜ける
		while (l_currentInfo)
		{
			// 自身の現在のIDと親のIDが一致すればtrueを返す
			if (l_currentInfo->k_id == a_baseTypeInfo.k_id)
			{
				return true;
			}

			// 自身のメンバに含まれる親を次のwhileで走査する対象として指定(継承ツリーを辿る)
			l_currentInfo = l_currentInfo->k_baseInfo;
		}

		return false;
	}
}