#pragma once

namespace FWK::Utility::String
{
	inline constexpr DWORD k_multiByteToWideCharFlags = 0UL;

	inline constexpr int k_multiByteToWideCharQueryBufferSize = 0;

	inline std::wstring StringToWideString(const std::string& a_string)
	{
		// 空文字列だった場合空std::wstringを返す
		if (a_string.empty()) { return std::wstring(); }

		// MultiByteToWideChar
		// (
		//		変換の実行に使用するコードページ（UTF-8など）、
		//		変換フラグ、
		//		文字列の先頭ポインタ、
		//		文字列のバイト数、
		//		出力用に格納するワイド文字列の先頭アドレス（nullptr = サイズ問い合わせ）、
		//		出力用ワイド文字列のサイズ				  （0       = サイズ取得モード）
		// )
		
		// UTF-8文字列をUTF-16(std::wstring)へ変換
		const int l_size = MultiByteToWideChar(CP_UTF8, 
											   k_multiByteToWideCharFlags,
											   a_string.data(),
											   static_cast<int>(a_string.size()),
											   nullptr,
											   k_multiByteToWideCharQueryBufferSize);
		
		if (l_size == 0)
		{
			assert(false && "MultiByteToWideChar(UTF-8からUTF-16への変換)に失敗しました。");
			return std::wstring();
		}

		// 出力バッファを確保しヌル文字で初期化
		std::wstring l_result(l_size, L'\0');

		// UTF-8 -> UTF-16へ実データ変換
		MultiByteToWideChar(CP_UTF8,
						    k_multiByteToWideCharFlags,
							a_string.data(),
						    static_cast<int>(a_string.size()),
							l_result.data(),
							l_size);

		return l_result;
	}
}