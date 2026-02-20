#pragma once

namespace FWK::Utility::String
{
	inline constexpr DWORD k_defaultMBFlags = 0UL;

	inline constexpr int   k_queryBufferSize = 0;

	inline std::wstring StringToWString(const std::string& a_string)
	{
		// 空文字列だった場合空"std::wstring"を返す
		if (a_string.empty()) { return std::wstring{}; }

		// "MultiByteToWideChar
		// (
		//		変換元文字コード（"UTF-8"）、
		//		変換フラグ（デフォルト動作）、
		//		入力"UTF-8"文字列の先頭ポインタ、
		//		入力文字列のバイト数、
		//		出力バッファ（"nullptr" "=" サイズ問い合わせ）
		//		出力バッファサイズ（"0" "=" サイズ取得モード）
		// )"
		
		// "UTF-8"文字列を"UTF-16(std::wstring)"へ変換
		const int l_size = MultiByteToWideChar(CP_UTF8, 
											   k_defaultMBFlags,
											   a_string.data   (),
											   static_cast<int>(a_string.size()),
											   nullptr,
											   k_queryBufferSize);
		
		// 出力バッファを確保し初期化
		std::wstring l_result(l_size, L'\0');

		// "UTF-8 -> UTF-16"へ実データ変換
		MultiByteToWideChar(CP_UTF8,
						    k_defaultMBFlags,
							a_string.data   (),
						    static_cast<int>(a_string.size()),
							l_result.data   (),
							l_size);

		return l_result;
	}
}