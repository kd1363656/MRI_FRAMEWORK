#pragma once

namespace FWK::Utility::FileIO
{
	inline constexpr int k_jsonIndentNum = 4;

	inline const char* const k_jsonExtension = ".json";

	inline nlohmann::json LoadJsonFile(const std::string& a_filePath)
	{
		std::filesystem::path l_path(a_filePath);

		// そもそもファイルが存在しなければreturn
		if (!std::filesystem::exists(l_path))
		{
			return nlohmann::json();
		}

		// 拡張子がjsonでない場合return
		if (l_path.extension() != k_jsonExtension)
		{
			return nlohmann::json();
		}

		// ファイルが空ならreturn
		if (std::filesystem::is_empty(l_path))
		{
			return nlohmann::json();
		}

		// ifstreamからjsonを読み込む
		std::ifstream l_ifs(l_path);

		if (l_ifs.fail())
		{
			return nlohmann::json();
		}

		// jsonをパースする
		auto l_loadedJson = nlohmann::json();

		l_ifs >> l_loadedJson; 

		return l_loadedJson;
	}

	inline void SaveJsonFile(const nlohmann::json& a_json, const std::string& a_filePath)
	{
		std::filesystem::path l_path(a_filePath);

		// 保存時、拡張子に.jsonがついていなければ拡張子をつける
		if (l_path.extension() != k_jsonExtension)
		{
			l_path += k_jsonExtension;
		}

		std::ofstream l_ofs(l_path, std::ios::out);

		l_ofs << a_json.dump(k_jsonIndentNum);
		l_ofs.close			();
	}

	inline std::string FetchExtension(const std::string& a_filePath)
	{
		if (a_filePath.empty())
		{
			return std::string();
		}

		const std::filesystem::path l_filePath (a_filePath);
		const std::filesystem::path l_extension = l_filePath.extension();

		if (l_extension.empty())
		{
			return std::string();
		}

		// stringに変換("."を含む形で帰る)
		return l_extension.string();
	}
}