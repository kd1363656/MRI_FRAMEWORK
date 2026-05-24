#pragma once

namespace FWK::Editor
{
	class EditorManager final : public SingletonBase<EditorManager>
	{
	public:

		void Init(const HWND& a_hwnd);

		void DrawEditor();

	private:

		static void AllocateSRVDescriptor(ImGui_ImplDX12_InitInfo* a_info, D3D12_CPU_DESCRIPTOR_HANDLE* a_outCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE* a_outGPUHandle);

		static void ReleaseSRVDescriptor(ImGui_ImplDX12_InitInfo* a_info, D3D12_CPU_DESCRIPTOR_HANDLE a_cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE a_gpuHandle);

		void Release();

		std::unordered_map<UINT64, TypeAlias::StorageID> m_srvStorageIDMap = {};

		bool m_isInitialized = false;

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<EditorManager>;

		 EditorManager();
		~EditorManager() override;
	};
}