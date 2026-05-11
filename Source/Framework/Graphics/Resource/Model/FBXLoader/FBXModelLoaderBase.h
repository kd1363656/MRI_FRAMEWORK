#pragma once

namespace FWK::Graphics
{
	class FBXModelLoaderBase
	{
	public:

				 FBXModelLoaderBase() = default;
		virtual ~FBXModelLoaderBase() = default;

	protected:

		ufbx_scene* LoadFBXScene(const std::filesystem::path& a_filePath) const;

		void DestroyFBXScene(ufbx_scene* a_fbxScene) const;

		TypeAlias::Math::Vector3 FetchVertexPosition(const ufbx_mesh* a_fbxMesh, const std::uint32_t a_vertexIndex) const;
		TypeAlias::Math::Vector3 FetchVertexNormal  (const ufbx_mesh* a_fbxMesh, const std::uint32_t a_vertexIndex) const;
		TypeAlias::Math::Vector2 FetchVertexUV      (const ufbx_mesh* a_fbxMesh, const std::uint32_t a_vertexIndex) const;

		TypeAlias::Math::Vector3 ConvertUFBXVector3ToVector3(const ufbx_vec3& a_fbxVector) const;
		TypeAlias::Math::Vector2 ConvertUFBXVector2ToVector2(const ufbx_vec2& a_fbxVector) const;

	private:

		static constexpr std::size_t k_errorTextBufferSize = 1024ULL;
	};
}