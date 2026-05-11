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

		StaticModelFBXLoader m_staticModelFBXLoader = {};
	};
}