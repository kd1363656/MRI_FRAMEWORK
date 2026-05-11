#include "FBXLoaderContext.h"

FWK::Graphics::FBXLoaderContext::FBXLoaderContext () = default;
FWK::Graphics::FBXLoaderContext::~FBXLoaderContext()
{
	Destroy();
}

bool FWK::Graphics::FBXLoaderContext::Create()
{
	return true;
}

bool FWK::Graphics::FBXLoaderContext::LoadStaticModelFile(const std::filesystem::path& a_filePath, Struct::ModelData& a_modelData) const
{
	return true;
}

void FWK::Graphics::FBXLoaderContext::Destroy()
{
}