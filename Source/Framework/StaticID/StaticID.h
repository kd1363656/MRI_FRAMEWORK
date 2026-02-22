#pragma once

namespace FWK
{
	// StaticIDのID生成元関数にアクセスされたくないためクラス化
	class StaticID final
	{
	public:

		// テンプレートを使った関数の特殊化を用いて動的に型ごとに一意なIDを生成している
		template <typename Type>
		static std::uint32_t GetTypeID()
		{
			static std::uint32_t l_id = GenerateTypeID();

			return l_id;
		}

	private:

		static std::uint32_t GenerateTypeID()
		{
			static std::uint32_t l_id = k_initialID;

			return l_id++;
		}
	
		// 0を明示的な無効な値として扱うため初めの値は1から行う
		static constexpr std::uint32_t k_initialID = 1u;
		
		// コンストラクタを削除することでこのクラスの生成を許さない
		StaticID () = delete;
		~StaticID() = delete;
	};
}