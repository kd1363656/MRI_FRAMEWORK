#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init  () const;
		bool Create();
		
		static constexpr UINT GetVALDefaultGPUNodeMask() { return k_defaultGPUNodeMask; }

	private:

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		// ID3D12Deviceを使って作るコマンドキューなどの各種GPUオブジェクトで
		// 共通使用する既定のGPUノード指定値
		// GPUノードマスクは「どのGPUノードを対象にするか」を表す値、
		// このプロジェクトでは単一GPU前提で作成するため固定値にしている
		static constexpr UINT k_defaultGPUNodeMask = 0U;

		Factory m_factory = {};
		Device  m_device  = {};

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		 GraphicsManager()          = default;
		~GraphicsManager() override = default;
	};
}