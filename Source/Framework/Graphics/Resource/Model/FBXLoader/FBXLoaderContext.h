#pragma once

namespace FWK::Graphics
{
	class FBXLoaderContext final
	{
	public:

		 FBXLoaderContext();
		~FBXLoaderContext();

		bool Create();

		bool LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const;

	private:

		void Destroy();

		FbxManager*    m_fbxManager    = nullptr;
		FbxIOSettings* m_fbxIOSettings = nullptr;

		StaticModelFBXLoader m_staticModelFBXLoader = {};
	};
}