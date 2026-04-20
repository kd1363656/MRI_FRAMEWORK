#pragma once

namespace FWK
{
	// 型ごとの静的ID取得のみを公開し、実際の採番処理は隠蔽するためのクラス
	class StaticTypeIDGenerator final
	{
	public:

		// テンプレートを使った関数の特殊化を用いて動的に型ごとに一意なIDを生成
		template <typename Type>
		static TypeAlias::StaticTypeID GetTypeID()
		{
			static auto l_id = GenerateTypeID();

			return l_id;
		}

	private:

		// 実際に静的IDを生成する関数
		static TypeAlias::StaticTypeID GenerateTypeID()
		{
			static auto l_id = k_initialStaticTypeID;

			return l_id++;
		}

		static constexpr TypeAlias::StaticTypeID k_initialStaticTypeID = 0U;
		
		// コンストラクタを削除することでこのクラスの生成を許さない
		 StaticTypeIDGenerator() = delete;
		~StaticTypeIDGenerator() = delete;
	};
}