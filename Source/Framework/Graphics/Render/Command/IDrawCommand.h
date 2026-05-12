#pragma once

namespace FWK::Graphics
{
	// Rendererクラスでそのフレームで必要な座標だったりサイズだったりを持つためのクラス
	class IDrawCommand
	{
	public:

				 IDrawCommand() = default;
		virtual ~IDrawCommand() = default;

		virtual void BeginFrame		()					   = 0;
		virtual void PostCreateSetup(Renderer& a_renderer) = 0;

		virtual void Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer) = 0;

		FWK_DEFINE_TYPE_INFO_ROOT(IDrawCommand);
	};
}