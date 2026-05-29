#include "SceneViewEditorWindow.h"

void FWK::Editor::SceneViewEditorWindow::Draw()
{
	const auto& l_windowName = GetTypeINFO().k_name.data();

	ImGui::Begin(l_windowName);

	const ImVec2 l_sceneViewSize = ImGui::GetContentRegionAvail();

	if (l_sceneViewSize.x <= k_minSceneViewSize ||
		l_sceneViewSize.y <= k_minSceneViewSize)
	{
		ImGui::End();
		return;
	}

	const ImTextureID& l_sceneViewTextureID = FetchVALSceneViewTextureID();

	if (!l_sceneViewTextureID)
	{
		ImGui::End();
		return;
	}

	DrawSceneViewTexture(l_sceneViewTextureID, l_sceneViewSize);

	ImGui::End();
}

ImTextureID FWK::Editor::SceneViewEditorWindow::FetchVALSceneViewTextureID() const
{
	const auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	const auto& l_renderer        = l_graphicsManager.GetREFRenderer();

	const auto& l_currentFrameResource = l_renderer.GetREFCurrentFrameResource().lock();

	if (!l_currentFrameResource)
	{
		return {};
	}

	const auto& l_renderGraphResourceRegistry = l_currentFrameResource->GetREFRenderGraphResourceRegistry();

	const auto& l_postEffectColorTextureRecord =
		l_renderGraphResourceRegistry.FindVALRenderTargetTexture(
			Utility::Tag::GetTag<Tag::PostEffectColorTextureTag>()
		).lock();

	if (!l_postEffectColorTextureRecord)
	{
		return {};
	}

	const auto& l_postEffectColorTexture = l_postEffectColorTextureRecord->m_renderTargetTexture;

	if (!l_postEffectColorTexture)
	{
		return {};
	}

	const TypeAlias::StorageID& l_srvStorageID = l_postEffectColorTexture->GetVALSRVStorageID();

	if (l_srvStorageID == Constant::k_invalidStorageID)
	{
		return {};
	}

	const auto& l_resourceContext   = l_graphicsManager.GetREFResourceContext();
	const auto& l_srvDescriptorPool = l_resourceContext.GetREFSRVDescriptorPool();

	const D3D12_GPU_DESCRIPTOR_HANDLE l_gpuHandle =
		l_srvDescriptorPool.FetchVALShaderVisibleGPUHandle(l_srvStorageID);

	return ConvertGPUHandleToImTextureID(l_gpuHandle);
}

ImTextureID FWK::Editor::SceneViewEditorWindow::ConvertGPUHandleToImTextureID(const D3D12_GPU_DESCRIPTOR_HANDLE& a_gpuHandle) const
{
	return static_cast<ImTextureID>(a_gpuHandle.ptr);
}

bool FWK::Editor::SceneViewEditorWindow::IsValidTextureID(const ImTextureID& a_textureID) const
{
	return a_textureID != static_cast<ImTextureID>(0);
}

void FWK::Editor::SceneViewEditorWindow::DrawSceneViewTexture(const ImTextureID& a_textureID, const ImVec2& a_sceneViewSize) const
{
	const ImVec2 l_uvMin = ImVec2(k_sceneViewUVMINX, k_sceneViewUVMINY);
	const ImVec2 l_uvMax = ImVec2(k_sceneViewUVMAXX, k_sceneViewUVMAXY);

	ImGui::Image(a_textureID, a_sceneViewSize, l_uvMin, l_uvMax);
}