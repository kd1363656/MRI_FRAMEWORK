#pragma once

namespace FWK::Utility::File
{
	// jsonファイルに保存する際のインデント幅
	inline constexpr int k_jsonIndentNum = 4;

	inline nlohmann::json LoadJsonFile(const std::filesystem::path& a_filePath)
	{
		// ファイルパスが存在しなければreturn
		if (!std::filesystem::exists(a_filePath)) { return nlohmann::json(); }
		
		// 通常ファイルでないならreturn
		if (!std::filesystem::is_regular_file(a_filePath)) { return nlohmann::json(); }

		// 拡張子が".json"出なければreturn
		if (a_filePath.extension() != CommonConstant::k_lowerJsonExtension) { return nlohmann::json(); }

		// ifstreamからjsonを読み込む
		std::ifstream l_ifs(a_filePath);

		// 読み込みに失敗したらreturn
		if (l_ifs.fail())
		{
			return nlohmann::json();
		}

		// jsonをパースする
		const auto& l_loadedJson = nlohmann::json::parse(l_ifs, nullptr, false);

		// jsonオブジェクトがパース失敗などで無効状態になっているかを確認し
		// 無効状態なら空のjsonを返す
		if (l_loadedJson.is_discarded())
		{
			return nlohmann::json();
		}

		return l_loadedJson;
	}

	inline void SaveJsonFile(const nlohmann::json& a_json, const std::filesystem::path& a_filePath)
	{
		// 拡張子が".json"出なければreturn
		if (a_filePath.extension() != CommonConstant::k_lowerJsonExtension) { return; }

		// 保存時、拡張子に.jsonがついていなければ拡張子をつける
		std::ofstream l_ofs(a_filePath, std::ios::out);

		l_ofs << a_json.dump(k_jsonIndentNum);
		l_ofs.close();
	}

	// 指定した拡張子に必要ならファイルパスの拡張子を変換する関数
	inline std::filesystem::path ReplaceFilePathExtensionIfNeed(const std::filesystem::path& a_filePath, const std::filesystem::path& a_replaceExtension)
	{
		// ファイルパス空ならreturn 
		if (a_filePath.empty()) { return a_filePath; }

		// ファイルパスの拡張子が指定した拡張子と同じならreturn
		if (a_filePath.extension() == a_replaceExtension) { return a_filePath; }

		auto l_result = a_filePath;

		l_result.replace_extension(a_replaceExtension);
		return l_result;
	}

	// 実ファイルの拡張子とプログラムで保存しているstd::filesystem::pathのファイルパスの拡張を変更する関数
	inline void RenameFileExtensionWithFilePathExtension(std::filesystem::path& a_sourcePath, const std::filesystem::path& a_replaceExtension)
	{
		// ファイルパスが空ならreturn
		if (a_sourcePath.empty()) { return; }

		// ファイルの存在が確認できなければreturn
		if (!std::filesystem::exists(a_sourcePath)) { return; }

		// 通常ファイルでなければreturn
		if (!std::filesystem::is_regular_file(a_sourcePath)) { return; }

		const auto l_newPath = ReplaceFilePathExtensionIfNeed(a_sourcePath, a_replaceExtension);

		// 同一のファイルパスなら変換する必要がないのでreturn
		if (a_sourcePath == l_newPath) { return; }

		auto l_ec = std::error_code();

		std::filesystem::rename(a_sourcePath, l_newPath, l_ec);

		if (l_ec)
		{
			assert(false && "ファイル名の変更に失敗しました");
			return;
		}

		// 最後にプログラム上のファイルパスも合わせる
		a_sourcePath = l_newPath;
	}
}