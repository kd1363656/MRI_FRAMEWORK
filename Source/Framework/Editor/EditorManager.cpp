#include "EditorManager.h"

FWK::Editor::EditorManager::EditorManager() = default;
FWK::Editor::EditorManager::~EditorManager()
{
	Release();
}

void FWK::Editor::EditorManager::Init(const HWND& a_hwnd)
{
	if (m_isInitialized) { return; }

	const auto& l_graphicsManager    = Graphics::GraphicsManager::GetInstance ();
	const auto& l_device		     = l_graphicsManager.GetREFDevice		  ().GetREFDevice();
	const auto& l_renderer			 = l_graphicsManager.GetREFRenderer		  ();
	const auto& l_resourceContext	 = l_graphicsManager.GetREFResourceContext();
	
	if (!l_device)
	{
		assert(false && "Deviceが無効のため、IMGUIの初期化に失敗しました。");
		return;
	}

	const auto& l_directCommandQueue = l_renderer.GetREFDirectCommandQueue().GetREFCommandQueue();

	if (!l_directCommandQueue)
	{
		assert(false && "コマンドキューが無効のため、IMGUIの初期化に失敗しました。");
		return;
	}

	const auto& l_srvDescriptorHeap = l_resourceContext.GetREFSRVDescriptorPool().GetREFDescriptorHeap().FetchVALShaderVisibleDescriptorHeap();

	if (!l_srvDescriptorHeap)
	{
		assert(false && "SRVDescriptorHeapが無効のため、IMGUIの初期化に失敗しました。");
		return;
	}

	const auto& l_frameResourceList = l_renderer.GetREFFrameResourceList();

	if (l_frameResourceList.empty()) 
	{
		assert(false && "フレームリソースリストが空のため、IMGUIの初期化に失敗しました。");
		return;
	}

	// IMGUIのバージョンをチェックして、IMGUIContextを作成する
	IMGUI_CHECKVERSION  ();
	ImGui::CreateContext();

	auto& l_io = ImGui::GetIO();

	l_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	// WIN32用IMGUIバックエンドを初期化する
	// ImGui_ImplWind32_Init(入力を受け取る対象ウィンドウハンドル);

	if (!ImGui_ImplWin32_Init(a_hwnd))
	{
		assert(false && "IMGUI_IMPLWIN32_INITに失敗しました。");
		return;
	}

	ImGui_ImplDX12_InitInfo l_initINFO = {};

	// DirectX12のDeviceを設定する
	l_initINFO.Device = l_device.Get();

	// IMGUI内部のテクスチャアップロードなどで使うDirectCommandQueueを設定する
	l_initINFO.CommandQueue = l_directCommandQueue.Get();

	// GPUとCPUで並行して使うフレーム数を設定する
	l_initINFO.NumFramesInFlight = static_cast<int>(l_frameResourceList.size());

	// メイン描画のRTVフォーマットを設定する
	l_initINFO.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 深度のフォーマット
	l_initINFO.DSVFormat = DXGI_FORMAT_UNKNOWN;

	// コールバック内でEditorManagerを取り出せるようにする
	l_initINFO.UserData = this;

	// IMGUIがSRVを保証するときに使うShaderVisibleなSRVDescriptorHeapを設定する
	l_initINFO.SrvDescriptorHeap = l_srvDescriptorHeap.Get();

	// IMGUIがSRVを確保するときに使うShaderVisibleなSRVDescriptorHeapを設定する
	l_initINFO.SrvDescriptorAllocFn = &EditorManager::AllocateSRVDescriptor;

	// IMGUIがsrvを開放するときに呼ばれる解放関数を設定する
	l_initINFO.SrvDescriptorFreeFn = &EditorManager::ReleaseSRVDescriptor;

	// DirectX12用IMGUIバックエンドを初期化する
	// ImGui_ImplDX12_Init(DirectX12用初期化情報);

	if (!ImGui_ImplDX12_Init(&l_initINFO))
	{
		assert(false && "ImGui_ImplDX12_Initに失敗しました。");
		return;
	}

	m_isInitialized = true;
}

void FWK::Editor::EditorManager::DrawEditor()
{
	if (!m_isInitialized) { return; }

	const auto& l_graphicsManager    = Graphics::GraphicsManager::GetInstance ();
	const auto& l_renderer			 = l_graphicsManager.GetREFRenderer		  ();
	const auto& l_directCommandList  = l_renderer.GetREFDirectCommandList	  ().GetREFCommandList();

	if (!l_directCommandList)
	{
		assert(false && "DirectCommandListが無効のため、IMGUIの描画処理に失敗しました。");
		return;
	}

	const auto& l_resourceContext   = l_graphicsManager.GetREFResourceContext  ();
	const auto& l_srvDescriptorHeap = l_resourceContext.GetREFSRVDescriptorPool().GetREFDescriptorHeap().FetchVALShaderVisibleDescriptorHeap();

	if (!l_srvDescriptorHeap)
	{
		assert(false && "SRVDescriptorHeapが無効のため、IMGUIの描画処理に失敗しました。");
		return;
	}

	// DirectX12用IMGUIバックエンドのフレーム開始処理
	ImGui_ImplDX12_NewFrame();

	// WIN32用IMGUIバックエンドのフレーム開始処理
	ImGui_ImplWin32_NewFrame();

	// IMGUIのフレーム開始
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	// IMGUIの描画データを確定する
	ImGui::Render();

	ID3D12DescriptorHeap* l_descriptorHeapList[] =
	{
		l_srvDescriptorHeap.Get()
	};

	// IMGUIのフォントテクスチャなどはSRVDescriptorHeapを使うため、描画前に設定する
	// SetDescriptorHeaps(設定するディスクリプタヒープ数、ディスクリプタヒープ配列);

	l_directCommandList->SetDescriptorHeaps(_countof(l_descriptorHeapList), l_descriptorHeapList);

	// IMGUIの描画コマンドを現在記録中のDirectCommandListへ積む
	// ImGui_ImplDX12_RenderDrawData(IMGUIが作成した描画データ、
	//								  描画命令を書き込むDirectCommandList);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), l_directCommandList.Get());
}

void FWK::Editor::EditorManager::AllocateSRVDescriptor(ImGui_ImplDX12_InitInfo* a_info, D3D12_CPU_DESCRIPTOR_HANDLE* a_outCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE* a_outGPUHandle)
{
	auto& l_graphicsManager    = Graphics::GraphicsManager::GetInstance		 ();
	auto& l_resourceContext	 = l_graphicsManager.GetMutableREFResourceContext();
	
	auto& l_srvDescriptorPool = l_resourceContext.GetMutableREFSRVDescriptorPool();

	if (!a_info			||
		!a_outCPUHandle ||
		!a_outGPUHandle) 
	{
		assert(false && "IMGUI用のSRVDescriptorの確保に失敗しました。");
		return;
	}

	auto* const l_editorManager = static_cast<EditorManager*>(a_info->UserData);

	if (!l_editorManager)
	{
		assert(false && "EditorManagerが無効のため、IMGUI用SRVDescriptorの確保に失敗しました。");
		return;
	}

	const auto l_srvStorageID = l_srvDescriptorPool.Allocate();

	if (l_srvStorageID == Constant::k_invalidStorageID)
	{
		assert(false && "ImGui用SRVDescriptorのStorageID確保に失敗しました。");
		return;
	}

	*a_outCPUHandle = l_srvDescriptorPool.FetchVALShaderVisibleCPUHandle(l_srvStorageID);
	*a_outGPUHandle = l_srvDescriptorPool.FetchVALShaderVisibleGPUHandle(l_srvStorageID);

	l_editorManager->m_srvStorageIDMap.try_emplace(a_outGPUHandle->ptr, l_srvStorageID);
}

void FWK::Editor::EditorManager::ReleaseSRVDescriptor(ImGui_ImplDX12_InitInfo* a_info, D3D12_CPU_DESCRIPTOR_HANDLE a_cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE a_gpuHandle)
{
	if (!a_info) { return; }

	auto* const l_editorManager = static_cast<EditorManager*>(a_info->UserData);

	if (!l_editorManager) { return; }

	auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();

	auto& l_srvDescriptorPool = l_resourceContext.GetMutableREFSRVDescriptorPool();

	const auto& l_itr = l_editorManager->m_srvStorageIDMap.find(a_gpuHandle.ptr);

	if (l_itr == l_editorManager->m_srvStorageIDMap.end()) { return; }

	l_srvDescriptorPool.Release(l_itr->second);

	l_editorManager->m_srvStorageIDMap.erase(l_itr);
}

void FWK::Editor::EditorManager::Release()
{
	if (!m_isInitialized) { return; }

	ImGui_ImplDX12_Shutdown ();
	ImGui_ImplWin32_Shutdown();

	if (ImGui::GetCurrentContext())
	{
		ImGui::DestroyContext();
	}

	auto& l_graphicsManager = Graphics::GraphicsManager::GetInstance();
	auto& l_resourceContext = l_graphicsManager.GetMutableREFResourceContext();

	auto& l_srvDescriptorPool = l_resourceContext.GetMutableREFSRVDescriptorPool();

	for (const auto& [l_gpuHandlePTR, l_storageID] : m_srvStorageIDMap)
	{
		l_srvDescriptorPool.Release(l_storageID);
	}

	m_srvStorageIDMap.clear();

	m_isInitialized = false;
}