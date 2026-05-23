#include "DrawStaticModelUnLitStandardCommand.h"

void FWK::Graphics::DrawStaticModelUnLitStandardCommand::Draw(const DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool, Renderer& a_renderer)
{
	// 描画用定数バッファのセットを行う
	DrawStaticModelStandardCommandBase::Draw(a_srvDescriptorPool, a_renderer);


}