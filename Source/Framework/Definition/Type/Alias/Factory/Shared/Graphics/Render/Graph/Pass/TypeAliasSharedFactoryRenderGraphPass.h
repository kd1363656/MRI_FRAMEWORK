#pragma once

namespace FWK::TypeAlias
{
	using SharedFactoryRenderGraphPass = GenericFactory<std::unique_ptr<Graphics::IRenderGraphPass>>;
}