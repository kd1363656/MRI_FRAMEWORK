#pragma once

namespace FWK::Utility::File
{
	inline constexpr int k_jsonIndentNum = 4;

	inline bool CanLoadFilePath(const std::filesystem::path& a_filePath, const std::filesystem::path& a_extension)
	{
		if (a_filePath.empty()) 
		{
			assert(false && "ファイルパスが空です、読み込めるファイルパスではありません。");
			return false; 
		}

		if (!std::filesystem::exists(a_filePath))
		{
			assert(false && "指定されたファイルパスが存在しません、読み込めるファイルパスではありません。");
			return false;
		}

		if (!std::filesystem::is_regular_file(a_filePath))
		{
			assert(false && "通常ファイルでないため、読み込めるファイルではありません。");
			return false;
		}

		if (a_filePath.extension() != a_extension)
		{
			assert(false && "ファイルパスの拡張子が一致しておりません、読み込めるファイルではありません。");
			return false;
		}

		return true;
	}

	inline bool CanLoadFilePath(const std::filesystem::path& a_filePath)
	{
		if (a_filePath.empty()) 
		{
			assert(false && "ファイルパスが空です、読み込めるファイルパスではありません。");
			return false; 
		}

		if (!std::filesystem::exists(a_filePath))
		{
			assert(false && "指定されたファイルパスが存在しません、読み込めるファイルパスではありません。");
			return false;
		}

		if (!std::filesystem::is_regular_file(a_filePath))
		{
			assert(false && "通常ファイルでないため、読み込めるファイルではありません。");
			return false;
		}

		return true;
	}

	inline std::filesystem::path CreateFilePathByReplaceExtension(const std::filesystem::path& a_filePath, const std::filesystem::path& a_extension)
	{
		auto l_filePath = a_filePath;

		// 指定された拡張子に置き換える
		l_filePath.replace_extension(a_extension);

		return l_filePath;
	}


	inline nlohmann::json LoadJsonFile(const std::filesystem::path& a_filePath)
	{
		// 読み込めないファイルならreturn
		if (!CanLoadFilePath(a_filePath, Constant::k_lowerJsonExtension)) { return {}; }

		// ifstreamからjsonを読み込む
		std::ifstream l_ifs{ a_filePath };

		// 読み込みに失敗したらreturn
		if (l_ifs.fail()) { return {}; }

		const auto& l_loadedJson = nlohmann::json::parse(l_ifs, nullptr, false);

		// jsonオブジェクトがパース失敗などで無効状態になっているかを確認し
		// 無効状態なら空のjsonを返す
		if (l_loadedJson.is_discarded()) { return {}; }

		return l_loadedJson;
	}

	inline void SaveJsonFile(const nlohmann::json& a_json, const std::filesystem::path& a_filePath)
	{
		// 拡張子が".json"でなければreturn
		if (a_filePath.extension() != Constant::k_lowerJsonExtension) { return; }

		std::ofstream l_ofs(a_filePath, std::ios::out);

		// ファイルパスにあるjsonにjsonデータを保存
		l_ofs << a_json.dump(k_jsonIndentNum);
		l_ofs.close         ();
	}
}