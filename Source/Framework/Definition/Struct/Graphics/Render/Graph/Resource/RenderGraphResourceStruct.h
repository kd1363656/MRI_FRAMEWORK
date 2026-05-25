#pragma once

namespace FWK::Struct
{
	struct RenderGraphRenderTargetTextureResourceRecord final
	{
		TypeAlias::TypeTag m_textureTag	= Constant::k_invalidTypeTag;
		
		std::shared_ptr<Graphics::RenderTargetTexture> m_renderTargetTexture = nullptr;
	};

	struct RenderGraphDepthStencilTextureResourceRecord final
	{
		TypeAlias::TypeTag m_textureTag = Constant::k_invalidTypeTag;

		std::shared_ptr<Graphics::DepthStencilTexture> m_depthStencilTexture = nullptr;
	};
}