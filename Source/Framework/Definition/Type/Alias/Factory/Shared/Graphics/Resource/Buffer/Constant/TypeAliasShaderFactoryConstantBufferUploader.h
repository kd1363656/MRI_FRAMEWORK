#pragma once

namespace FWK::TypeAlias
{
	using ShaderFactoryConstantBufferUploader = GenericFactory<std::shared_ptr<Graphics::ConstantBufferUploaderBase>>;
}