#include "CameraConstantBuffer.h"

FWK::Graphics::CameraConstantBufferUploader::CameraConstantBufferUploader() : 
	ConstantBufferUploaderBase(k_bufferTypeSize)
{}
FWK::Graphics::CameraConstantBufferUploader::~CameraConstantBufferUploader() = default;