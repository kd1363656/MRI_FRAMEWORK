#pragma once

namespace FWK::Graphics
{
	class ModelSystem final
	{
	public:

		 ModelSystem() = default;
		~ModelSystem() = default;

		bool Create();

		bool LoadStaticModel(const std::filesystem::path& a_filePath) const;

	private:

		FBXModelLoader      m_fbxModelLoader      = {};
		ModelMeshletBuilder m_modelMeshletBuilder = {};
	};
}