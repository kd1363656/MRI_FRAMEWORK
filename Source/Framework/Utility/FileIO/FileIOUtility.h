#pragma once

namespace FWK::Utility::FileIO
{
	inline constexpr int k_jsonIndentNum = 4;

	inline const char* const k_jsonExtension = ".json";

	inline nlohmann::json LoadJsonFile(const std::string& a_filePath)
	{
		std::filesystem::path l_path{ a_filePath };

		// そもそもファイルが存在しなければreturn
		if (!std::filesystem::exists(l_path))
		{
			return {};
		}

		// 拡張子がjsonでない場合return
		if (l_path.extension() != k_jsonExtension)
		{
			return {};
		}

		// ファイルが空ならreturn
		if (std::filesystem::is_empty(l_path))
		{
			return {};
		}

		// ifstreamからjsonを読み込む
		std::ifstream l_ifs(l_path);

		if (l_ifs.fail())
		{
			return {};
		}

		// jsonをパースする
		auto l_loadedJson = nlohmann::json{};
		l_ifs >> l_loadedJson; 

		return l_loadedJson;
	}

	inline void SaveJsonFile(const nlohmann::json& a_json, const std::string& a_filePath)
	{
		std::filesystem::path l_path{ a_filePath };

		// 保存時、拡張子に.jsonがついていなければ拡張子をつける
		if (l_path.extension() != k_jsonExtension)
		{
			l_path += k_jsonExtension;
		}

		std::ofstream l_ofs{ l_path, std::ios::out };

		l_ofs << a_json.dump(k_jsonIndentNum);
		l_ofs.close			();
	}
}