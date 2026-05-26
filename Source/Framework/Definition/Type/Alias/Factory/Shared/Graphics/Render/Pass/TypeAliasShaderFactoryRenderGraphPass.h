#pragma once

namespace FWK::TypeAlias
{
	using ShaderFactoryRenderGraphPass = GenericFactory<std::unique_ptr<Graphics::IRenderGraphPass>>;
}