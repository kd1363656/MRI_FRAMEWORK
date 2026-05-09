#pragma once

namespace FWK::Graphics
{
	class StaticModelFBXLoader final : public FBXLoaderBase
	{
	public:

		 StaticModelFBXLoader()			 = default;
		~StaticModelFBXLoader() override = default;

		bool LoadStaticModelFile(const std::filesystem::path& a_filePath, FbxManager* a_fbxManager) const;

	private:

		int CountMeshNode(FbxNode* a_fbxNode) const;

		static constexpr int k_emptyMeshCount = 0;
	};
}