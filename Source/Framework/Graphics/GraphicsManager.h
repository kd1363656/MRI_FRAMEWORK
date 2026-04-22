#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init      () const;
		void LoadCONFIG();
		bool Create    (const HWND& a_hwnd, const Struct::WindowCONFIG& a_windowCONFIG);
		
		void SaveCONFIG() const;

		static constexpr UINT GetVALDefaultGPUNodeMask() { return k_defaultGPUNodeMask; }

		const auto& GetREFResourceContext() const { return m_resourceContext; }
		const auto& GetREFRenderer       () const { return m_renderer; }
		const auto& GetREFSwapChain      () const { return m_swapChain; }

		auto& GetREFMutableResourceContext() { return m_resourceContext; }
		auto& GetREFMutableRenderer       () { return m_renderer; }
		auto& GetREFMutableSwapChain	  () { return m_swapChain; }

	private:

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		// ID3D12Deviceを使って作るコマンドキューなどの各種GPUオブジェクトで
		// 共通使用する既定のGPUノード指定値
		// GPUノードマスクは「どのGPUノードを対象にするか」を表す値、
		// このプロジェクトでは単一GPU前提で作成するため固定値にしている
		static constexpr UINT k_defaultGPUNodeMask = 0U;

		const std::filesystem::path k_configFileIOPath = "Asset/Data/CONFIG/Graphics/GraphicsCONFIG.json";

		Factory         m_factory         = {};
		Device          m_device          = {};
		ResourceContext m_resourceContext = {};
		Renderer        m_renderer		  = {};
		SwapChain		m_swapChain		  = {};

		JsonConverter::GraphicsManagerJsonConverter m_graphicsManagerJsonConverter = {};
		
		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		 GraphicsManager()          = default;
		~GraphicsManager() override = default;
	};
}