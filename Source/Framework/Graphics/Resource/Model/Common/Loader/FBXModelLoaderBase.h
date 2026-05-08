#pragma once

namespace FWK::Graphics
{
	class FBXModelLoaderBase
	{
	public:

				 FBXModelLoaderBase();
		virtual ~FBXModelLoaderBase();

		bool Create();

	protected:

		TypeAlias::Math::Vector3 FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const;

		TypeAlias::Math::Vector3 FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const;

		TypeAlias::Math::Vector2 FetchVertexUV(const FbxMesh* a_fbxMesh, 
											   const int      a_polygonIndex,
											   const int	  a_polygonVertexIndex,
											   const char*	  a_uvSetName) const;

		TypeAlias::Math::Vector3 ConvertFbxVector4ToVector3(const FbxVector4& a_fbxVector) const;


		FbxManager* GetPTRFBXManager() const { return m_fbxManager; }

	private:

		void Destroy();

		static constexpr float k_uvVFlipOffset = 1.0F;

		static constexpr int k_minControlPointIndex =  0;
		
		static constexpr int k_fbxVectorXIndex = 0;
		static constexpr int k_fbxVectorYIndex = 1;
		static constexpr int k_fbxVectorZIndex = 2;

		static constexpr int k_fbxUVUIndex = 0;
		static constexpr int k_fbxUVVIndex = 1;

		FbxManager* m_fbxManager;
	};
}