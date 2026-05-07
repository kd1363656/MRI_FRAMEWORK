#include "ModelMeshletBuilder.h"

bool FWK::Graphics::ModelMeshletBuilder::BuildStaticModelMeshletData(const Struct::StaticModelMesh& a_staticModelMesh, Struct::ModelMeshletData& a_modelMeshlettData) const
{
	if (a_staticModelMesh.m_staticModelVertexList.empty())
	{
		assert(false && "StaticModelMeshの頂点リストが空のため、ModelMeshletDataの作成に失敗しました。");
		return false;
	}

	if (a_staticModelMesh.m_indexList.empty())
	{
		assert(false && "StaticModelMeshのインデックスリストが空のため、ModelMeshletDataの作成に失敗しました。");
		return false;
	}

	a_modelMeshlettData = {};

	return true;
}