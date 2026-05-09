#pragma once

namespace FWK::Graphics
{
	class FBXLoaderBase
	{
	public:

				 FBXLoaderBase() = default;
		virtual ~FBXLoaderBase() = default;

	protected:

		FbxScene* ImportScene(const std::filesystem::path& a_filePath, FbxManager* a_fbxManager) const;
		
	private:

		static constexpr std::string_view k_defaultSceneName    = "ModelScene";
		static constexpr std::string_view k_defaultImporterName = "ModelImporter";

		static constexpr int  k_autoDetectFileFormat = -1;
	};
}