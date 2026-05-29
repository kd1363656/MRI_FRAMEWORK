#include "SceneViewEditorWindow.h"

void FWK::Editor::SceneViewEditorWindow::Draw()
{
	// SceneView用のIMGUIウィンドウを開始する
	if (!ImGui::Begin(k_editorName.data())) 
	{
		ImGui::End();
		return; 
	}
	
	// 現在のSceneViewウィンドウ内で、実際に画像を表示できる領域サイズを取得する
	// Dockingでウィンドウサイズが変わると、この値も変わる
	const ImVec2& l_sceneViewSize = ImGui::GetContentRegionAvail();

	if (l_sceneViewSize.x <= k_minSceneViewSize ||
		l_sceneViewSize.y <= k_minSceneViewSize)
	{
		// 無効サイズの場合は描画をしない
		ImGui::End();
		return;
	}

	// RenderGraphで作成された最終カラーTextureを、IMGUIで表示できるTextureIDとして取得する
	// 今回はRenderTargetTextureの解像度変更は行わなず、既存の描画結果をSceneView内に拡縮表示する
	const auto l_sceneViewTextureID = FetchVALSceneViewTextureID();

	// SceneViewの表示領域全体に、取得したTextureを描画する
	DrawSceneViewTexture(l_sceneViewTextureID, l_sceneViewSize);

	ImGui::End();
}

ImTextureID FWK::Editor::SceneViewEditorWindow::FetchVALSceneViewTextureID() const
{
	// GraphicsManagerからRendererへアクセスし、現在フレームのRendererGraphリソースを取得する
	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer		  = l_graphicsManager.GetREFRenderer	  ();

	// 現在描画に使用しているFrameResourceを取得する
	const auto& l_currentFrameResource = l_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource) { return k_invalidSceneViewTextureID; }

	// RenderGraphで管理されているRenderTargetTextureなどのリソース登録情報を取得する
	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();

	// PostEffectColorTextureTagが付いたRenderTargetTextureTagを探す
	// これはポストエフェクト後の最後カラー画像として扱う
	const auto& l_postEffectColorTextureRecord = l_renderGraphResourceRegistry.FindVALRenderTargetTexture(Utility::Tag::GetTag<Tag::PostEffectColorTextureTag>()).lock();

	if (!l_postEffectColorTextureRecord) { return k_invalidSceneViewTextureID; }

	// RenderTargetTexture本体を取得する
	const auto& l_postEffectColorTexture = l_postEffectColorTextureRecord->m_renderTargetTexture;

	if (!l_postEffectColorTexture) { return k_invalidSceneViewTextureID; }

	// IGUIでTextureを表示するにはTextureをShaderから読めるSRVが必要になる
	// ここではREnderTargetTextureに割り当てられているSRVのStorageIDを取得する
	const TypeAlias::StorageID& l_srvStorageID = l_postEffectColorTexture->GetVALSRVStorageID();

	if (l_srvStorageID == Constant::k_invalidStorageID) { return k_invalidSceneViewTextureID; }

	// SRVDescriptorPoolからShaderVisibleなGPUDescriptorHandleを取得する
	// IMGUIのDirectX12バックエンドでは、このGPUHandleをImTextureIDとして渡す
	const auto& l_resourceContext   = l_graphicsManager.GetREFResourceContext  ();
	const auto& l_srvDescriptorPool = l_resourceContext.GetREFSRVDescriptorPool();

	const auto l_gpuHandle = l_srvDescriptorPool.FetchVALShaderVisibleGPUHandle(l_srvStorageID);

	return ConvertGPUHandleToImTextureID(l_gpuHandle);
}

ImTextureID FWK::Editor::SceneViewEditorWindow::ConvertGPUHandleToImTextureID(const D3D12_GPU_DESCRIPTOR_HANDLE& a_gpuHandle) const
{
	return a_gpuHandle.ptr;
}

void FWK::Editor::SceneViewEditorWindow::DrawSceneViewTexture(const ImTextureID& a_textureID, const ImVec2& a_sceneViewSize) const
{
	// Texture全体を表示するため、UV範囲は左上から右下までを指定する
	const auto l_uvMIN = ImVec2(k_sceneViewUVMINX, k_sceneViewUVMINY);
	const auto l_uvMAX = ImVec2(k_sceneViewUVMAXX, k_sceneViewUVMAXY);

	// 取得したTextureをSceneViewの表示領域いっぱいに描画する
	// a_sceneViewSizeにGetContentRegionAvailの値を渡しているため、Dockingの拡縮に追従する
	ImGui::Image(a_textureID, a_sceneViewSize, l_uvMIN, l_uvMAX);
}