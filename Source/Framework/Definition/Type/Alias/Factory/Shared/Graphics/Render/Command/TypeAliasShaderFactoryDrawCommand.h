#pragma once

namespace FWK::TypeAlias
{
	using ShaderFactoryDrawCommand = GenericFactory<std::shared_ptr<Graphics::DrawCommandBase>>;
}