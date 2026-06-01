#include "CameraPassConstantBufferUploader.h"

FWK::Graphics::CameraPassConstantBufferUploader::CameraPassConstantBufferUploader() : 
	ConstantBufferUploaderBase(k_bufferTypeSize)
{}
FWK::Graphics::CameraPassConstantBufferUploader::~CameraPassConstantBufferUploader() = default;