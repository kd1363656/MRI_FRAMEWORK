#pragma once

namespace FWK::Graphics
{
	class TextureSystem
}

namespace FWK::Graphics
{
	// Rendererクラスでそのフレームで必要な座標だったりサイズだったりを持つためのクラス
	class IDrawCommand
	{
	public:

				 IDrawCommand() = default;
		virtual ~IDrawCommand() = default;

		virtual void BeginFrame() = 0;

		virtual void Draw(const Renderer& a_renderer, const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, TextureSystem& a_textureSystem) = 0;

		FWK_DEFINE_TYPE_INFO_ROOT(IDrawCommand);
	};
}