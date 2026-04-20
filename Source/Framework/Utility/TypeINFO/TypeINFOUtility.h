#pragma once

namespace FWK::Utility::TypeINFO
{
	// 指定された方を継承しているかどうかを確認する関数
	// 継承ツリーが深ければ位深いほど処理が遅くなる
	inline bool IsDerivedFromBase(const Struct::TypeINFO& a_typeINFO, const Struct::TypeINFO& a_baseTypeINFO)
	{
		// 参照のポインタを取得しているのでnullチェックは行わない
		const auto* l_currentINFO = &a_typeINFO;

		// 自身がnullptrを示すつまり、a_typeInfoが継承している親まで走査し終えたらwhile文を抜ける
		while (l_currentINFO)
		{
			// 自身の現在のIDと親のIDが一致すればtrueを返す
			if (l_currentINFO->k_staticTypeID == a_baseTypeINFO.k_staticTypeID) { return true; }

			// 自身のメンバに含まれる親を次のwhileで走査する対象として指定(継承ツリーを辿る)
			l_currentINFO = l_currentINFO->k_baseINFO;
		}

		return false;
	}
}