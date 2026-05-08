#include "FBXModelLoaderBase.h"

FWK::Graphics::FBXModelLoaderBase::FBXModelLoaderBase() : 
	m_fbxManager(nullptr)
{}
FWK::Graphics::FBXModelLoaderBase::~FBXModelLoaderBase()
{
	Destroy();
}

bool FWK::Graphics::FBXModelLoaderBase::Create()
{
	if (m_fbxManager) { return true; }

	// FBXSDK全体の管理オブジェクトを作成する
	// FbxManager::Create();
	m_fbxManager = FbxManager::Create();

	if (!m_fbxManager)
	{
		assert(false && "FbxManagerの作成処理に失敗したため、FBXModelLoaderの作成に失敗しました。");
		return false;
	}

	// FBXSDKの読み込み設定を作成する
	// FbxIOSettings::Create(FbxManager,
	//						 設定名);
	auto* l_fbxIOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);

	if (!l_fbxIOSettings)
	{
		Destroy();

		assert(false && "FbxIOSettingsの作成に失敗したため、FBXModelLoaderの作成に失敗しました。");
		return false;
	}

	// FbxManagerに読み込み設定を登録する
	// SetIOSettings(使用するFBX読み込み設定)
	m_fbxManager->SetIOSettings(l_fbxIOSettings);

	return true;
}

void FWK::Graphics::FBXModelLoaderBase::Destroy()
{
	if (!m_fbxManager) { return; }

	m_fbxManager->Destroy();

	m_fbxManager = nullptr;
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoaderBase::FetchVertexPosition(const FbxMesh* a_fbxMesh, const int a_controlPointIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	if (a_controlPointIndex < k_minControlPointIndex ||
		a_controlPointIndex >= a_fbxMesh->GetControlPointsCount())
	{
		assert(false && "ControlPointIndexが範囲外のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	const auto* l_controlPointList = a_fbxMesh->GetControlPoints();

	if (!l_controlPointList)
	{
		assert(false && "FBXメッシュのControlPointListが無効のため、頂点座標の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	const auto& l_controlPoint = l_controlPointList[a_controlPointIndex];

	return ConvertFbxVector4ToVector3(l_controlPoint);
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoaderBase::FetchVertexNormal(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、法線の取得に失敗しました。");
		return TypeAlias::Math::Vector3::Zero;
	}

	FbxVector4 l_fbxNormal = {};

	// GetPolygonVertexNormal(ポリゴン番号、
	//						  ポリゴン内の頂点番号、
	//						  取得した法線の格納先);

	if (!a_fbxMesh->GetPolygonVertexNormal(a_polygonIndex, a_polygonVertexIndex, l_fbxNormal)) { return TypeAlias::Math::Vector3::Zero; }

	return ConvertFbxVector4ToVector3(l_fbxNormal);
}

FWK::TypeAlias::Math::Vector2 FWK::Graphics::FBXModelLoaderBase::FetchVertexUV(const FbxMesh* a_fbxMesh, const int a_polygonIndex, const int a_polygonVertexIndex, const char* a_uvSetName) const
{
	if (!a_fbxMesh)
	{
		assert(false && "FbxMeshが無効のため、UVの取得に失敗しました。");
		return TypeAlias::Math::Vector2::Zero;
	}

	if (!a_uvSetName) { return TypeAlias::Math::Vector2::Zero; }

	FbxVector2 l_fbxUV = {};

	bool l_isUnmapped = false;

	// GetPolygonVertexUV(ポリゴン番号、
	//					  ポリゴン内の頂点番号、
	//					  使用するUVセット名、
	//					  取得したUVの格納先、
	//					  UVが未割り当てかどうかの格納先);
	if (!a_fbxMesh->GetPolygonVertexUV(a_polygonIndex,
									   a_polygonVertexIndex,
									   a_uvSetName,
									   l_fbxUV,
									   l_isUnmapped))
	{
		return TypeAlias::Math::Vector2::Zero;
	}

	if (l_isUnmapped) { return TypeAlias::Math::Vector2::Zero; }

	return TypeAlias::Math::Vector2
	{
		static_cast<float>(l_fbxUV[k_fbxUVUIndex]),
		k_uvVFlipOffset - static_cast<float>(l_fbxUV[k_fbxUVVIndex])
	};
}

FWK::TypeAlias::Math::Vector3 FWK::Graphics::FBXModelLoaderBase::ConvertFbxVector4ToVector3(const FbxVector4& a_fbxVector) const
{
	return TypeAlias::Math::Vector3
	{
		static_cast<float>(a_fbxVector[k_fbxVectorXIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorYIndex]),
		static_cast<float>(a_fbxVector[k_fbxVectorZIndex])
	};
}