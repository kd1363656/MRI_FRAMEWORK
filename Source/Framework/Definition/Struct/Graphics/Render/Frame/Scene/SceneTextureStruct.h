#pragma once

namespace FWK::Struct
{
	struct SceneRenderTargetTextureRecord final
	{
		TypeAlias::TypeTag m_typeTag = Constant::k_invalidTypeTag;

		std::shared_ptr<Graphics::RenderTargetTexture> m_renderTargetTexture = nullptr;
	};

	struct SceneDepthStencilTextureRecord final
	{
		TypeAlias::TypeTag m_typeTag = Constant::k_invalidTypeTag;

		std::shared_ptr<Graphics::DepthStencilTexture> m_depthStencilTexture = nullptr;
	};
}