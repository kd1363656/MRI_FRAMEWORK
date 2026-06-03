#include "DeferredLightingPassConstantBufferUploader.h"

FWK::Graphics::DeferredLightingPassConstantBufferUploader::DeferredLightingPassConstantBufferUploader() : 
	ConstantBufferUploaderBase(k_bufferTypeSize)
{}
FWK::Graphics::DeferredLightingPassConstantBufferUploader::~DeferredLightingPassConstantBufferUploader() = default;