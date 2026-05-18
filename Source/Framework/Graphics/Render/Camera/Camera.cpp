#include "Camera.h"

void FWK::Graphics::Camera::SetupPerspective(const TypeAlias::Math::Matrix& a_cameraMatrix,
											 const float					a_aspectRatio,
											 const float					a_fovYDegree, 
										     const float					a_farClip, 
											 const float					a_nearClip)
{
	// カメラ行列と射影行列をセット
	SetCameraMatrix(a_cameraMatrix);

	SetProjectionMatrix(a_aspectRatio,
						a_fovYDegree,
						a_farClip,
						a_nearClip);
}

void FWK::Graphics::Camera::SetCameraMatrix(const TypeAlias::Math::Matrix& a_cameraMatrix)
{
	m_cameraMatrix = a_cameraMatrix;

	// カメラ行列の逆行列をViewMatrixとして作成する
	// CameraMatrix : カメラのWorldMatrix
	// ViewMatrix   : World空間をカメラ空間へ変換する行列
	m_viewMatrix = m_cameraMatrix.Invert();

	UpdateViewProjectionMatrix();
}

void FWK::Graphics::Camera::SetProjectionMatrix(const float a_aspectRatio,
												const float a_fovYDegree,
												const float a_farClip,
												const float a_nearClip)
{
	if (a_aspectRatio <= k_invalidAspectRatio)
	{
		assert(false && "CameraのAspectRatioが不正なため、ProjectionMatrixの作成に失敗しました。");
		return;
	}

	// X<ConvertToRadians(度数法の角度);
	// 度数法で指定された視野角を、DirectXMathが扱うラジアンへ変換する
	const float l_fovYRadian = DirectX::XMConvertToRadians(a_fovYDegree);

	// CreatePerspectiveFieldOfView(縦方向の視野角、
	//								画面の横縦比、
	//							    近クリップ、
	//								遠クリップ);
	// DirectX12で扱いやすい左手系のProjectionMatrixを作成する
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(l_fovYRadian,
														   a_aspectRatio,
														   a_nearClip,
														   a_farClip);

	UpdateViewProjectionMatrix();
}

void FWK::Graphics::Camera::SetProjectionMatrix(const TypeAlias::Math::Matrix& a_projectionMatrix)
{
	m_projectionMatrix = a_projectionMatrix;

	UpdateViewProjectionMatrix();
}

FWK::Struct::CBCamera FWK::Graphics::Camera::CreateCBCamera() const
{
	Struct::CBCamera l_cbCamera = {};

	l_cbCamera.m_viewMatrix		      = m_viewMatrix;
	l_cbCamera.m_projectionMatrix     = m_projectionMatrix;
	l_cbCamera.m_viewProjectionMatrix = m_viewProjectionMatrix;

	return l_cbCamera;
}

void FWK::Graphics::Camera::UpdateViewProjectionMatrix()
{
	m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
}