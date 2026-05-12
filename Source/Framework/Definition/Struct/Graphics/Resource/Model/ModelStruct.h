#pragma once

namespace FWK::Struct
{
	struct ModelVertex final
	{
		TypeAlias::Math::Vector3 m_position = {};
		TypeAlias::Math::Vector3 m_normal   = {};
		TypeAlias::Math::Vector2 m_uv		= {};
	};

	struct ModelMaterialAssetData final
	{
		 ModelMaterialAssetData() = default;
		~ModelMaterialAssetData() = default;

		 ModelMaterialAssetData(const ModelMaterialAssetData&)           = default;
		 ModelMaterialAssetData(	  ModelMaterialAssetData&&) noexcept = default;

		 ModelMaterialAssetData& operator=(const ModelMaterialAssetData&)		    = default;
		 ModelMaterialAssetData& operator=(	     ModelMaterialAssetData&&) noexcept = default;
		 
		std::wstring m_baseColorTextureFilePath = {};
		std::wstring m_normalTextureFilePath    = {};
		std::wstring m_roughnessTextureFilePath = {};
		std::wstring m_metallicTextureFilePath  = {};
	};

	struct ModelMaterialRuntimeData final
	{
		 ModelMaterialRuntimeData() = default;
		~ModelMaterialRuntimeData() = default;

		ModelMaterialRuntimeData(const ModelMaterialRuntimeData&)			= delete;
		ModelMaterialRuntimeData(	   ModelMaterialRuntimeData&&) noexcept = default;

		ModelMaterialRuntimeData& operator=(const ModelMaterialRuntimeData&)		   = delete;
		ModelMaterialRuntimeData& operator=(	  ModelMaterialRuntimeData&&) noexcept = default;

		Graphics::Texture m_baseColorTexture = {};
		Graphics::Texture m_normalTexture    = {};
		Graphics::Texture m_roughnessTexture = {};
		Graphics::Texture m_metallicTexture  = {};
	};

	struct ModelMaterial final
	{
		 ModelMaterial() = default;
		~ModelMaterial() = default;

		ModelMaterial(const ModelMaterial&)           = delete;
		ModelMaterial(	    ModelMaterial&&) noexcept = default;

		ModelMaterial& operator=(const ModelMaterial&)			 = delete;
		ModelMaterial& operator=(	   ModelMaterial&&) noexcept = default;

		ModelMaterialAssetData   m_modelMaterialAssetData   = {};
		ModelMaterialRuntimeData m_modelMaterialRuntimeData = {};
	};

	struct ModelMesh final
	{
		 ModelMesh() = default;
		~ModelMesh() = default;

		ModelMesh(const ModelMesh&)			  = delete;
		ModelMesh(	    ModelMesh&&) noexcept = default;

		ModelMesh& operator=(const ModelMesh&)			 = delete;
		ModelMesh& operator=(	   ModelMesh&&) noexcept = default;

		std::vector<ModelVertex>   m_modelVertexList = {};
		std::vector<std::uint32_t> m_indexList	     = {};

		ModelMaterial m_modelMaterial = {};
	};

	struct ModelData final
	{
		 ModelData() = default;
		~ModelData() = default;

		ModelData(const ModelData&)			  = delete;
		ModelData(		ModelData&&) noexcept = default;

		ModelData& operator=(const ModelData&)			 = delete;
		ModelData& operator=(	   ModelData&&) noexcept = default;

		std::vector<ModelMesh> m_modelMeshList = {};
	};
}